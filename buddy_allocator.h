#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "bitmap_tree.h"

typedef struct {
    BitmapTree *bitmap;
    void *base;
    size_t size;
    size_t min_block;
} BuddyAllocator;

BuddyAllocator* buddy_new(size_t size, size_t min_block);

void buddy_init(BuddyAllocator *allocator, size_t size, size_t min_block);

void *buddy_alloc(size_t size);

void buddy_free(void* ptr);

int buddy_is_valid_pointer(BuddyAllocator *allocator, void* ptr);

#endif