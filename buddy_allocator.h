#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>

#include "bitmap_tree.h"

typedef struct {
    size_t allocations;
    size_t deallocations;
    size_t failed_allocations;
    size_t splits;
    size_t merges;
    size_t used_memory;
} BuddyStats;

typedef struct {
    BitmapTree *bitmap;
    void *base;
    size_t size;
    size_t min_block;
    BuddyStats stats;
} BuddyAllocator;

BuddyAllocator* buddy_new(size_t size, size_t min_block);
void buddy_delete(BuddyAllocator *allocator);

void buddy_init(BuddyAllocator *allocator, size_t size, size_t min_block);

void *buddy_alloc(BuddyAllocator *allocator, size_t size);

void buddy_free(BuddyAllocator *allocator, void* ptr);

void buddy_dump_tree(BuddyAllocator *allocator);

void buddy_print_stats(BuddyAllocator *allocator);

bool buddy_is_valid_pointer(BuddyAllocator *allocator, void* ptr);

#endif