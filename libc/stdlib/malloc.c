#include <stdint.h>
#include <string.h>

#if defined (__is_libk)
    #include <kernel/heap.h>
    void* malloc(size_t size) {
        return (void*) kmalloc(size);
    }

    void free(void* ptr) {
        kfree((uintptr_t) ptr);
    }

    void* realloc(void* ptr, size_t size) {
        return (void*) krealloc((uintptr_t) ptr, size);
    }

    void* calloc(size_t num, size_t size) {
        if (num <= 0 || size <= 0) return 0;
        uint64_t r = num * size;
        if (r > 0xFFFFFFFF) return 0;
        uintptr_t ptr = kmalloc(num * size);
        memset((void*)ptr, 0, num*size);
        return (void*)ptr;
    }
#else

#endif