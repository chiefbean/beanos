#include <kernel/tty.h>
#include <sys/isr.h>
#include <sys/idt.h>
#include <stdio.h>

#include <stdint.h>

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_install() {
    set_idt_gate(0, (uint32_t)&isr0);
    set_idt_gate(1, (uint32_t)&isr1);
    set_idt_gate(2, (uint32_t)&isr2);
    set_idt_gate(3, (uint32_t)&isr3);
    set_idt_gate(4, (uint32_t)&isr4);
    set_idt_gate(5, (uint32_t)&isr5);
    set_idt_gate(6, (uint32_t)&isr6);
    set_idt_gate(7, (uint32_t)&isr7);
    set_idt_gate(8, (uint32_t)&isr8);
    set_idt_gate(9, (uint32_t)&isr9);
    set_idt_gate(10, (uint32_t)&isr10);
    set_idt_gate(11, (uint32_t)&isr11);
    set_idt_gate(12, (uint32_t)&isr12);
    set_idt_gate(13, (uint32_t)&isr13);
    set_idt_gate(14, (uint32_t)&isr14);
    set_idt_gate(15, (uint32_t)&isr15);
    set_idt_gate(16, (uint32_t)&isr16);
    set_idt_gate(17, (uint32_t)&isr17);
    set_idt_gate(18, (uint32_t)&isr18);
    set_idt_gate(19, (uint32_t)&isr19);
    set_idt_gate(20, (uint32_t)&isr20);
    set_idt_gate(21, (uint32_t)&isr21);
    set_idt_gate(22, (uint32_t)&isr22);
    set_idt_gate(23, (uint32_t)&isr23);
    set_idt_gate(24, (uint32_t)&isr24);
    set_idt_gate(25, (uint32_t)&isr25);
    set_idt_gate(26, (uint32_t)&isr26);
    set_idt_gate(27, (uint32_t)&isr27);
    set_idt_gate(28, (uint32_t)&isr28);
    set_idt_gate(29, (uint32_t)&isr29);
    set_idt_gate(30, (uint32_t)&isr30);
    set_idt_gate(31, (uint32_t)&isr31);

    set_idt(); // Load with ASM

    for(uint8_t i = 0; i < 32; i++) {
        register_interrupt_handler(i, &isr_handler);
    }
}

/* To print the message which defines every exception */
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(registers_t r) {
    printf("received interrupt: %d %s\n", r.int_no, exception_messages[r.int_no]);
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
  interrupt_handlers[n] = handler;
} 