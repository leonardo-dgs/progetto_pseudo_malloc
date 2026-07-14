#ifndef MMAP_ALLOCATOR_H
#define MMAP_ALLOCATOR_H

void *mmap_alloc(size_t size);

void mmap_free(void *ptr, size_t size);

#endif