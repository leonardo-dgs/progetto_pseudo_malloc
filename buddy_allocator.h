#include "bitmap_tree.h"

typedef struct {
    BitmapTree *bitmap;
    void *base;
    size_t size;
    size_t min_block;
} BuddyAllocator;

void buddy_init(BuddyAllocator *allocator, size_t size, size_t min_block);

void *buddy_alloc(size_t size);

void buddy_free(void* ptr);

int size_to_order();

void split_block();

void merge_block();

int find_free_block();
