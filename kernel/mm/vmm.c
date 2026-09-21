#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <kernel/boot.h>

void invlpg() {
    __asm__ __volatile__("movl %%cr3, %%eax\n\t" "movl %%eax, %%cr3" ::: "memory");
}

void vmm_init() {
    uint32_t cr3_val;
    __asm__ __volatile__ ("mov %%cr3, %0":"=r" (cr3_val)::);

    uint32_t* pd = (uint32_t*) PHYS_TO_VIRT(cr3_val);
    pd[1023] = cr3_val | PAGE_PRESENT | PAGE_RW;

    invlpg();
}

int map_page(uintptr_t virt, uintptr_t phys, uint16_t flags) {
    uint32_t pd_index = virt >> 22;
    uint32_t pt_index = (virt >> 12) & 0x3FF;
    uint32_t* pd = (uint32_t*) RECURSIVE_PD;
    uint32_t* pt = (uint32_t*) (RECURSIVE_PT_BASE + pd_index * 0x1000);

    if (!(pd[pd_index] & PAGE_PRESENT)) {
        uint32_t new_pt = alloc_frame();
        if (new_pt == 0) return 0;
        pd[pd_index] = new_pt | PAGE_PRESENT | PAGE_RW;
        invlpg();
        for (size_t i = 0; i < 1024; i++) pt[i] = 0;
    }

    pt[pt_index] = (phys & PAGE_FRAME_MASK) | flags | PAGE_PRESENT;

    invlpg();

    return 1;
}

void unmap_page(uintptr_t virt) {
    uint32_t pd_index = virt >> 22;
    uint32_t pt_index = (virt >> 12) & 0x3FF;
    uint32_t* pt = (uint32_t*) (RECURSIVE_PT_BASE + pd_index * 0x1000);
    uint32_t phys = pt[pt_index] & PAGE_FRAME_MASK;

    pt[pt_index] = 0;

    invlpg();

    free_frame(phys);
}