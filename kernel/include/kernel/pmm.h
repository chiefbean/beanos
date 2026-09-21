#ifndef _KERNEL_PMM_H
#define _KERNEL_PMM_H 1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void pmm_init();
uintptr_t alloc_frame();
void free_frame(uintptr_t frame);

#ifdef __cplusplus
}
#endif

#endif