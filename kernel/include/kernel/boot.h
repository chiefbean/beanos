#ifndef _KERNEL_BOOT_H
#define _KERNEL_BOOT_H 1

#include <stdint.h>
#include <stddef.h>

#define KERNEL_VIRT_BASE 0xC0000000
#define PHYS_TO_VIRT(p) (uintptr_t)(p + KERNEL_VIRT_BASE)
#define VIRT_TO_PHYS(v) ((uintptr_t)(v) - KERNEL_VIRT_BASE)

typedef struct {
    uint64_t base;
    uint64_t length;
    int usable;
} mem_region;

typedef struct {
    mem_region regions[32];
    size_t count;
    uint64_t total_usable;
    uint32_t mem_lower;
    uint32_t mem_upper;
} boot_info;

extern boot_info g_boot_info;

void boot_info_parse(uint32_t magic, void* mbi);

#endif