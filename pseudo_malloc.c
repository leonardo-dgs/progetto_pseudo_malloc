#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "buddy_allocator.h"
#include "mmap_allocator.h"

#include "pseudo_malloc.h"

typedef enum {
    ALLOCATOR_BUDDY,
    ALLOCATOR_MMAP
} AllocatorType;

typedef struct {
    AllocatorType type;
    size_t size;
} BlockHeader;

static BuddyAllocator *buddy_allocator;

void set_header(void *ptr, AllocatorType type, size_t size);

void *pseudo_malloc(size_t size) {
    if (size <= 0) {
        fprintf(stderr, "pseudo_malloc: invalid size\n");
        return NULL;
    }
    if (buddy_allocator == NULL) {
        buddy_allocator = buddy_new(BUDDY_SIZE, BUDDY_BLOCK);
    }

    size_t page_size = sysconf(_SC_PAGESIZE);
    void *alloc = NULL;
    if (size < page_size / 4) {
        alloc = buddy_alloc(buddy_allocator, size + sizeof(BlockHeader));
        set_header(alloc, ALLOCATOR_BUDDY, size);
    }
    if (alloc == NULL) {
        alloc = mmap_alloc(sizeof(BlockHeader) + size);
        set_header(alloc, ALLOCATOR_MMAP, size);
    }

    if (alloc == NULL) {
        fprintf(stderr, "pseudo_malloc: not enough memory\n");
        return NULL;
    }
    return alloc + sizeof(BlockHeader);
}

void pseudo_free(void *pointer) {
    if (pointer == NULL) {
        fprintf(stderr, "pseudo_free: null pointer supplied\n");
        return;
    }

    BlockHeader *block_header = (BlockHeader *) (pointer - sizeof(BlockHeader));
    if (block_header->type == ALLOCATOR_BUDDY) {
        buddy_free(buddy_allocator, pointer);
    }
    else {
        mmap_free(pointer, sizeof(BlockHeader) + header->size);
    }
}

void set_header(void *ptr, AllocatorType type, size_t size) {
    if (ptr == NULL)
        return;
    BlockHeader *block_header = (BlockHeader *) ptr;
    block_header->type = type;
    block_header->size = size;
}
