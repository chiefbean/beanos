#include <kernel/boot.h>
#include <stddef.h>
#include <stdio.h>

#include "multiboot.h"

boot_info g_boot_info;

void boot_info_parse(uint32_t magic, void* mbi) {
    if (magic != 0x2BADB002) {
        printf("\nPANIC: Magic does not match expected value.");
        for(;;) {
            __asm__ __volatile__("hlt");
        }
    }

    multiboot_info_t* g_mbi = (multiboot_info_t*) PHYS_TO_VIRT(mbi);
    size_t count = 0;

    if (g_mbi->flags & (1 << 6)) {
        // walk mmap
        uintptr_t m_addr = PHYS_TO_VIRT(g_mbi->mmap_addr);
        while (m_addr < PHYS_TO_VIRT(g_mbi->mmap_addr + g_mbi->mmap_length)) {
            multiboot_memory_map_t* entry = (multiboot_memory_map_t*) m_addr;
            mem_region reg = {entry->addr, entry->len, (entry->type == 1)};
            g_boot_info.total_usable += (reg.usable) ? entry->len : 0;
            g_boot_info.regions[count++] = reg;

            if (count >= 32) {
                printf("\nOut of mappable memory regions.");
                break;
            }

            m_addr += entry->size + sizeof(entry->size);
        }

        g_boot_info.count = count;
    } else {
        // fallback
        mem_region reg = {0x100000, g_mbi->mem_upper * 1024, 1};
        g_boot_info.mem_lower = g_mbi->mem_lower;
        g_boot_info.mem_upper = g_mbi->mem_upper;
        g_boot_info.total_usable = g_mbi->mem_upper * 1024;
        g_boot_info.regions[0] = reg;
        g_boot_info.count = 1;

        printf("%p: %u - %c", reg.base, reg.length, reg.usable ? 'T' : 'F');
    }
}