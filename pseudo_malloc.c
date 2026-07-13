#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "pseudo_malloc.h"

typedef struct {
    size_t size;
} BlockHeader;

static BuddyAllocator *buddy_allocator;

void *pseudo_malloc(size_t size) {
    if (size <= 0) {
        fprintf(stderr, "pseudo_malloc: invalid size\n");
        return NULL;
    }
    if (buddy_allocator == NULL) {
        buddy_allocator = buddy_new(BUDDY_SIZE, BUDDY_BLOCK);
    }
    size_t page_size = sysconf(_SC_PAGESIZE);
    if (size < page_size / 4) {
        void *alloc = buddy_alloc(buddy_allocator, size);
        if (alloc == NULL) {
            fprintf(stderr, "pseudo_malloc: not enough memory\n");
        }
        return alloc;
    }
    else {
        void *alloc = mmap(NULL, sizeof(BlockHeader) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        BlockHeader *header = (BlockHeader *) alloc;
        header->size = size;
        return header + 1;
    }
}

void pseudo_free(void *pointer) {
    if (pointer == NULL) {
        fprintf(stderr, "pseudo_free: null pointer supplied\n");
        return;
    }
    if (buddy_is_valid_pointer(allocator, pointer)) {
        buddy_free(buddy_allocator, pointer);
    }
    else {
        BlockHeader *header = ((BlockHeader *) pointer) - 1;
        munmap(header, sizeof(BlockHeader) + header->size);
    }
}
