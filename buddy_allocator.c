#include <stdlib.h>
#include <math.h>

#include "buddy_allocator.h"

int pointer_to_index(BuddyAllocator *allocator, void* ptr);
int size_to_order(BuddyAllocator *allocator, size_t size);
int index_to_order(BuddyAllocator *allocator, size_t index);
int buddy_alloc_r(BuddyAllocator *allocator, int node_index, int target_order);

void buddy_init(BuddyAllocator *allocator, size_t size, size_t min_block) {
    size_t number_of_blocks = size / min_block;
    buddy_alloc->base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    buddy_alloc->bitmap = mmap(NULL, sizeof(BitmapTree), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    bitmaptree_init(buddy_alloc->bitmap, number_of_blocks / 8);
    buddy_alloc->size = size;
    buddy_alloc->min_block = min_block;
}

void *buddy_alloc(BuddyAllocator *allocator, size_t size) {
    int target_order = size_to_order(allocator, size);
    int index = buddy_alloc_r(allocator, 0, target_order);
    if (index < 0)
        return NULL;
    return (char*) allocator->base + index * allocator->min_block;
}

void buddy_free(BuddyAllocator *allocator, void* ptr) {
    int index = pointer_to_index(allocator, ptr);
    bitmaptree_mark_free(allocator->bitmap, index);
    int buddy_index = bitmaptree_buddy(allocator->bitmap, index);
    if (bitmaptree_is_free(allocator->bitmap, buddy_index)) {
        int parent_index = bitmaptree_parent(allocator->bitmap, index);
        bitmaptree_mark_free(parent_index);
    }
}

int buddy_alloc_r(BuddyAllocator *allocator, int node_index, int target_order) {
    int order = index_to_order(allocator, node_index);
    if (order < target_order)
        return -1;
    if (!bitmaptree_is_free(allocator->bitmap, node_index) && !bitmaptree_is_split(allocator->bitmap, node_index))
        return -1;
    if (bitmaptree_is_split(allocator->bitmap, node_index)) {
        int left_child_index = bitmaptree_left_child(allocator->bitmap, node_index);
        int right_child_index = bitmaptree_right_child(allocator->bitmap, node_index);
        int free_index = buddy_alloc_r(allocator, left_child_index, target_order);
        if (free_index < 0)
            free_index = buddy_alloc_r(allocator, right_child_index, target_order);
        if (free_index < 0)
            return -1;
        return free_index;
    }
    if (bitmaptree_is_free(allocator->bitmap, node_index)) {
        if (order == target_order) {
            bitmaptree_mark_used(allocator->bitmap, node_index);
            return node_index;
        }
        bitmaptree_mark_split(allocator->bitmap, node_index);
        int left_child_index = bitmaptree_left_child(allocator->bitmap, node_index);
        return buddy_alloc_r(allocator, left_child_index, target_order);
    }
    return -1;
}

int pointer_to_index(BuddyAllocator *allocator, void* ptr) {
    return ((char*) ptr - (char*) allocator->base) / allocator->min_block;
}

int size_to_order(BuddyAllocator *allocator, size_t size) {
    return (int) ceil(log2(size / allocator->min_block));
}

int index_to_order(BuddyAllocator *allocator, size_t index) {
    int int max_order = allocator->size / allocator->min_block;
    int level = (int) floor(log2(index + 1));
    return max_order - level;
}
