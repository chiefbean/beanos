#include <kernel/pmm.h>
#include <kernel/boot.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

uint32_t PMM_MAPPED_LIMIT = 0x3FF000;
uint32_t FRAME_ROUND_MASK = 0xFFFFF000;
uint32_t FRAME_ROUND_MAX = 0xFFF;

extern char _kernel_start[];
extern char _kernel_end[];

static uintptr_t free_stack_top = 0;
static size_t free_count = 0;

int is_reserved(uintptr_t frame_phys) {
    if (frame_phys < 0x100000) return 1;

    uintptr_t k_start_round = (VIRT_TO_PHYS(_kernel_start)) & FRAME_ROUND_MASK;
    uintptr_t k_end_round = (VIRT_TO_PHYS(_kernel_end) + FRAME_ROUND_MAX) & FRAME_ROUND_MASK;

    if (frame_phys >= k_start_round && frame_phys < k_end_round) return 1;

    return 0;
}

void pmm_init() {

    for(size_t i = 0; i < g_boot_info.count; i++) {
        mem_region r = g_boot_info.regions[i];

        if (r.usable) {
            uintptr_t start = r.base;
            uintptr_t end = r.base + r.length;

            if (end > PMM_MAPPED_LIMIT) end = PMM_MAPPED_LIMIT;

            if (start >= end) continue;

            start = (start + FRAME_ROUND_MAX) & FRAME_ROUND_MASK;
            end = end & FRAME_ROUND_MASK;

            for(uintptr_t i = start; i < end; i += 4096) {
                if (!is_reserved(i)) free_frame(i);
            }
        }
    }
    if ((free_count * 4096) > g_boot_info.total_usable) {
        printf("\nPANIC: More free pages than mappable memory.");
        for(;;) {
            __asm__ __volatile__("hlt");
        }
    }
}

uintptr_t alloc_frame() {
    if (free_stack_top == 0) return 0;

    uintptr_t phys = free_stack_top;
    free_stack_top = *(uintptr_t*)PHYS_TO_VIRT(phys);
    free_count--;

    return phys;
}

void free_frame(uintptr_t phys) {
    uintptr_t* virt_addr = (uintptr_t*)PHYS_TO_VIRT(phys);
    *virt_addr = free_stack_top;
    free_stack_top = phys;
    free_count++;
}