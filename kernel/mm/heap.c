#include <kernel/heap.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <stdio.h>
#include <string.h>

uintptr_t heap_end = HEAP_START;

int grow_heap(size_t size, heap_node_t* node) {
    uint32_t start = heap_end;
    uint32_t mapped = 0;

    while (mapped < (size + sizeof(heap_node_t))) {
        if (heap_end + 4096 > HEAP_MAX) return 0;
        uintptr_t phys = alloc_frame();
        if (!phys) return 0;
        map_page(heap_end, phys, PAGE_RW);
        heap_end += 4096;
        mapped += 4096;
    } 

    if(node->free) {
        node->size += mapped;
    } else {
        *(volatile heap_node_t*) start = (heap_node_t) {mapped - sizeof(heap_node_t), 1, NULL, node};
        node->next = (heap_node_t*) start;
    }
    
    return 1;
}

void heap_init() {
    map_page(HEAP_START, alloc_frame(), PAGE_RW);
    heap_end += 4096;
    *(volatile heap_node_t*) HEAP_START = (heap_node_t) {4096 - sizeof(heap_node_t), 1, NULL, NULL};
}

uintptr_t kmalloc(size_t size) {
    int found = 0;
    size_t aligned;
    if (size > 8) aligned = ALIGN_SIZE(size, 16);
    else aligned = ALIGN_SIZE(size, 8);

    heap_node_t* node = (heap_node_t*) HEAP_START;
    heap_node_t* last;
    do {
        last = node;
        if (node->free && node->size >= aligned) {
            found = 1;
            break;
        } else node = node->next;
    } while(node != NULL);

    if (!found) {
        if (!grow_heap(aligned, last)) return 0;

        node = (heap_node_t*) HEAP_START;
        do {
            if (node->free && node->size >= aligned) {
                found = 1;
                break;
            } else node = node->next;
        } while(node != NULL);
        if (!found) return 0;
    }

    if (node->size > (aligned + sizeof(heap_node_t) + MIN_PAYLOAD)) {
        heap_node_t* split = (heap_node_t*)((uint8_t*)(node+1) + aligned);
        *split = (heap_node_t) {node->size - aligned - sizeof(heap_node_t), 1, node->next, node};
        if (node->next) node->next->prev = split;
        node->next = split;
        node->size = aligned;
    }

    node->free = 0;

    return (uintptr_t) (node + 1);
}

int kfree(uintptr_t ptr) {
    if (!ptr) return 0;
    heap_node_t* node = (heap_node_t*)((uint8_t*)(ptr) - sizeof(heap_node_t));
    node->free = 1;

    if (node->next && node->next->free) {
        node->size += node->next->size + sizeof(heap_node_t);
        node->next = node->next->next;
        if (node->next) node->next->prev = node;
    }
    if (node->prev && node->prev->free) {
        node->prev->size += node->size + sizeof(heap_node_t);
        node->prev->next = node->next;
        if (node->next) node->next->prev = node->prev;
    }

    return 1;
}

uintptr_t krealloc(uintptr_t ptr, size_t size) {
    if (ptr == (uintptr_t) NULL) {
        return kmalloc(size);
    } else if(size == 0) {
        kfree(ptr);
        return (uintptr_t) NULL;
    }
    
    heap_node_t* node = (heap_node_t*)((uint8_t*)(ptr) - sizeof(heap_node_t));
    if (node->size >= size) return ptr;

    if (node->next && node->next->free && node->size + node->next->size > size) {
        node->size += node->next->size + sizeof(heap_node_t);
        node->next = node->next->next;
        if (node->next) node->next->prev = node;

        return ptr;
    }

    uintptr_t new = kmalloc(size);
    if (!new) return 0;

    memcpy((void*)new, (void*)ptr, node->size < size ? node->size : size);
    kfree(ptr);

    return new;
}