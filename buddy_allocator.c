#include <stdlib.h>
#include <math.h>

#include "buddy_allocator.h"

static BuddyError last_error;

int buddy_alloc_r(BuddyAllocator *allocator, int node_index, int target_order);
void buddy_free_r(BuddyAllocator *allocator, int index);
int pointer_to_index(BuddyAllocator *allocator, void* ptr);
int is_power_of_two(size_t x);

BuddyAllocator* buddy_new(size_t size, size_t min_block) {
    if (min_block > size) {
        fprintf(stderr, "buddy_new: min_block must be minor than size\n");
        return;
    }
    if (!is_power_of_two(size) || !is_power_of_two(min_block)) {
        fprintf(stderr, "buddy_new: size and min_block must be power of two\n");
        return;
    }
    BuddyAllocator *allocator = mmap(NULL, sizeof(BuddyAllocator), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    buddy_init(allocator, size, min_block);
    return allocator;
}

void buddy_init(BuddyAllocator *allocator, size_t size, size_t min_block) {
    if (min_block > size) {
        fprintf(stderr, "buddy_init: min_block must be minor than size\n");
        return;
    }
    if (!is_power_of_two(size) || !is_power_of_two(min_block)) {
        fprintf(stderr, "buddy_init: size and min_block must be power of two\n");
        return;
    }
    size_t number_of_blocks = size / min_block;
    buddy_alloc->base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    buddy_alloc->bitmap = bitmaptree_new(number_of_blocks / 8, min_block);
    buddy_alloc->size = size;
    buddy_alloc->min_block = min_block;
}

void *buddy_alloc(BuddyAllocator *allocator, size_t size) {
    int target_order = bitmaptree_size_to_order(allocator->bitmap, size);
    int index = buddy_alloc_r(allocator, 0, target_order);
    if (index < 0)
        return NULL;
    return (char*) allocator->base + index * allocator->min_block;
}

void buddy_free(BuddyAllocator *allocator, void* ptr) {
    buddy_free_r(allocator, pointer_to_index(allocator, ptr));
}

int buddy_is_valid_pointer(BuddyAllocator *allocator, void* ptr) {
    if (allocator == NULL || ptr == NULL)
        return 0;
    return ptr >= allocator->base && ptr < allocator->base + allocator->size;
}

int buddy_alloc_r(BuddyAllocator *allocator, int node_index, int target_order) {
    int order = bitmaptree_index_to_order(allocator->bitmap, node_index);
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

void buddy_free_r(BuddyAllocator *allocator, int index) {
    bitmaptree_mark_free(allocator->bitmap, index);
    if (!bitmaptree_is_root(index)) {
        int buddy_index = bitmaptree_buddy(allocator->bitmap, index);
        if (bitmaptree_is_free(allocator->bitmap, buddy_index)) {
            int parent_index = bitmaptree_parent(allocator->bitmap, index);
            buddy_free_r(allocator, parent_index);
        }
    }
}

int pointer_to_index(BuddyAllocator *allocator, void* ptr) {
    return ((char*) ptr - (char*) allocator->base) / allocator->min_block;
}

int is_power_of_two(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}
