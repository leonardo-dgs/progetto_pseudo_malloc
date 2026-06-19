#include <stdlib.h>

#include "buddy_allocator.h"

void buddy_init(BuddyAllocator *allocator, size_t size, size_t min_block) {
    size_t number_of_blocks = size / min_block;
    buddy_alloc->base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    buddy_alloc->bitmap = mmap(NULL, sizeof(BitmapTree), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    bitmaptree_init(buddy_alloc->bitmap, number_of_blocks / 8);
    buddy_alloc->size = size;
    buddy_alloc->min_block = min_block;
}

void *buddy_alloc(size_t size) {

}

void buddy_free(void* ptr) {

}

int size_to_order() {

}

void split_block() {

}

void merge_block() {

}

int find_free_block() {

}
