#ifndef _KERNEL_VMM_H
#define _KERNEL_VMM_H 1

#include <stddef.h>
#include <stdint.h>

#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4
#define PAGE_FRAME_MASK 0xFFFFF000

#define RECURSIVE_PD 0xFFFFF000
#define RECURSIVE_PT_BASE 0xFFC00000

#ifdef __cplusplus
extern "C" {
#endif


void invlpg();
void vmm_init();
int map_page(uintptr_t virt, uintptr_t phys, uint16_t flags);
void unmap_page(uintptr_t virt);

#ifdef __cplusplus
}
#endif

#endif