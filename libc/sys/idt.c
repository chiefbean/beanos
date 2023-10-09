#include <sys/idt.h>
#include <stdint.h>

extern void idt_flush(uint32_t);

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = (uint16_t)handler & 0xFFFF;
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; 
    idt[n].high_offset = (uint16_t)handler >> 16;
}

void set_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    /* Don't make the mistake of loading &idt -- always load &idt_reg */
    __asm__ __volatile__("lidt %0" : : "m"(idt_reg));
    __asm__ __volatile__("sti");
}