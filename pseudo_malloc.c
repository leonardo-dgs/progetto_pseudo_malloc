#include <stdlib.h>
#include <unistd.h>

#include "pseudo_malloc.h"

static BuddyAllocator *buddy_alloc;

void *pseudo_malloc(size_t size) {
    if (buddy_alloc == NULL) {
        buddy_alloc = mmap(NULL, sizeof(BuddyAllocator), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        buddy_init(buddy_alloc, BUDDY_SIZE, BUDDY_BLOCK);
    }
    size_t page_size = sysconf(_SC_PAGESIZE);
    if (size < page_size / 4) {
        return buddy_alloc(size);
    }
    else {
        void *alloc = mmap(NULL, sizeof(BlockHeader) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        BlockHeader *header = (BlockHeader *) alloc;
        header->size = size;
        return header + 1;
    }
}

void pseudo_free(void *pointer) {
    BlockHeader *header = ((BlockHeader *) pointer) - 1;
    munmap(header, sizeof(BlockHeader) + header->size);
}
