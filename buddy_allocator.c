#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <math.h>

#include "buddy_allocator.h"

int buddy_alloc_r(BuddyAllocator *allocator, size_t node_index, size_t target_order);
void buddy_free_r(BuddyAllocator *allocator, size_t index);
int pointer_to_index(BuddyAllocator *allocator, void* ptr);
void* index_to_pointer(BuddyAllocator *allocator, size_t index);
int is_power_of_two(size_t x);

BuddyAllocator* buddy_new(size_t size, size_t min_block) {
    if (size <= 0 || min_block <= 0) {
        fprintf(stderr, "buddy_new: size and min_block must be greater than zero\n");
        return NULL;
    }
    if (min_block > size) {
        fprintf(stderr, "buddy_new: min_block must be minor than size\n");
        return NULL;
    }
    if (!is_power_of_two(size) || !is_power_of_two(min_block)) {
        fprintf(stderr, "buddy_new: size and min_block must be power of two\n");
        return NULL;
    }
    BuddyAllocator *allocator = mmap(NULL, sizeof(BuddyAllocator), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (allocator == MAP_FAILED) {
        fprintf(stderr, "buddy_new: mmap failed\n");
        return NULL;
    }
    buddy_init(allocator, size, min_block);
    return allocator;
}

void buddy_init(BuddyAllocator *allocator, size_t size, size_t min_block) {
    if (allocator == NULL) {
        fprintf(stderr, "buddy_alloc: the supplied parameter BuddyAllocator is null\n");
        return;
    }
    if (size <= 0 || min_block <= 0) {
        fprintf(stderr, "buddy_new: size and min_block must be greater than zero\n");
        return;
    }
    if (min_block > size) {
        fprintf(stderr, "buddy_init: min_block must be minor than size\n");
        return;
    }
    if (!is_power_of_two(size) || !is_power_of_two(min_block)) {
        fprintf(stderr, "buddy_init: size and min_block must be power of two\n");
        return;
    }
    size_t number_of_blocks = size / min_block;
    size_t bitmap_size = ceil((double) (2 * number_of_blocks - 1) / 8);
    allocator->base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    allocator->bitmap = bitmaptree_new(bitmap_size, min_block);
    allocator->size = size;
    allocator->min_block = min_block;
}

void buddy_delete(BuddyAllocator *allocator) {
    bitmaptree_delete(allocator->bitmap);
    munmap(allocator->base, allocator->size);
    munmap(allocator, sizeof(BuddyAllocator));
}

void *buddy_alloc(BuddyAllocator *allocator, size_t size) {
    if (allocator == NULL) {
        fprintf(stderr, "buddy_alloc: the supplied parameter BuddyAllocator is null\n");
        return NULL;
    }
    if (size <= 0) {
        fprintf(stderr, "buddy_alloc: invalid size supplied\n");
        return NULL;
    }
    int target_order = bitmaptree_size_to_order(allocator->bitmap, size);
    int index = buddy_alloc_r(allocator, 0, target_order);
    if (index < 0)
        return NULL;
    return index_to_pointer(allocator, index);
}

void buddy_free(BuddyAllocator *allocator, void* ptr) {
    if (!buddy_is_valid_pointer(allocator, ptr)) {
        fprintf(stderr, "buddy_free: invalid parameters supplied\n");
        return;
    }
    buddy_free_r(allocator, pointer_to_index(allocator, ptr));
}

int buddy_is_valid_pointer(BuddyAllocator *allocator, void* ptr) {
    if (allocator == NULL || ptr == NULL)
        return 0;
    unsigned char *p = ptr;
    unsigned char *base = allocator->base;
    if (p < base || p >= base + allocator->size)
        return 0;
    size_t difference = p - base;
    if (difference % allocator->min_block != 0)
        return 0;
    return !bitmaptree_is_free(allocator->bitmap, pointer_to_index(allocator, ptr)) && !bitmaptree_is_split(allocator->bitmap, pointer_to_index(allocator, ptr));
}

int buddy_alloc_r(BuddyAllocator *allocator, size_t node_index, size_t target_order) {
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

void buddy_free_r(BuddyAllocator *allocator, size_t index) {
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
    size_t difference = (unsigned char*) ptr - (unsigned char*) allocator->base;
    size_t base_index = allocator->size / allocator->min_block - 1;
    size_t leaf_block_index = difference / allocator->min_block;
    size_t index = base_index + leaf_block_index;
    while (index > 0 && (bitmaptree_is_free(allocator->bitmap, index) || bitmaptree_is_split(allocator->bitmap, index))) {
        index = bitmaptree_parent(allocator->bitmap, index);
    }
    return index;
}

void* index_to_pointer(BuddyAllocator *allocator, size_t index) {
    size_t level = bitmaptree_index_to_level(allocator->bitmap, index);
    size_t number_of_blocks_at_level = 1 << level;
    size_t block_size_at_level = allocator->size / number_of_blocks_at_level;
    size_t block_index_at_level = index - ((1 << level) - 1);
    return (char *) allocator->base + block_index_at_level * block_size_at_level;
}

int is_power_of_two(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}
