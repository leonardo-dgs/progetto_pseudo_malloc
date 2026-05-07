#include <stdlib.h>
#include <unistd.h>

#include "pseudo_malloc.h"

static buddy_alloc_t *buddy_alloc;

void *pseudo_malloc(size_t size) {
    if (buddy_alloc == NULL) {
        buddy_alloc = mmap(NULL, sizeof(buddy_alloc_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        buddy_alloc->base = mmap(NULL, BUDDY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        buddy_alloc->bitmap = mmap(NULL, BUDDY_BLOCK, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        buddy_alloc->size = BUDDY_SIZE;
    }
    size_t page_size = sysconf(_SC_PAGESIZE);
    if (size < page_size / 4) {

    }
    else {
        void *alloc = mmap(NULL, sizeof(block_header_t) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        block_header_t *header = (block_header_t *) alloc;
        header->size = size;
        return header + 1;
    }
}

void pseudo_free(void *pointer) {
    block_header_t *header = ((block_header_t *) pointer) - 1;
    munmap(header, sizeof(block_header_t) + header->size);
}
