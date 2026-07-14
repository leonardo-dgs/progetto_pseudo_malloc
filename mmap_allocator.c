#include <stdlib.h>

#include "mmap_allocator.h"

void *mmap_alloc(size_t size) {
    void *alloc = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (alloc == MAP_FAILED)
        return NULL;
    return alloc;
}

void mmap_free(void *ptr, size_t size) {
    munmap(pointer, size);
}
