#ifndef _KERNEL_HEAP_H
#define _KERNEL_HEAP_H 1

#define HEAP_START 0xC0400000
#define HEAP_MAX 0xC0800000
#define MIN_PAYLOAD 16

#define ALIGN_SIZE(size, alignment) (((size) + (alignment) - 1) & ~((alignment) - 1))

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void heap_init();
uintptr_t kmalloc(size_t size);
int kfree(uintptr_t ptr);
uintptr_t krealloc(uintptr_t ptr, size_t size);

typedef struct heap_node_t {
    size_t size;
    size_t free;
    struct heap_node_t* next;
    struct heap_node_t* prev;
} heap_node_t;

#ifdef __cplusplus
}
#endif

#endif