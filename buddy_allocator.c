#include <stdio.h>
#include <sys/mman.h>
#include <limits.h>

#include "intmath.h"

#include "buddy_allocator.h"

ptrdiff_t buddy_alloc_r(BuddyAllocator *allocator, size_t node_index, size_t target_order);
void buddy_free_r(BuddyAllocator *allocator, size_t index);
size_t pointer_to_index(BuddyAllocator *allocator, void* ptr);
void* index_to_pointer(BuddyAllocator *allocator, size_t index);

BuddyAllocator* buddy_new(size_t size, size_t min_block) {
    if (size <= 0 || min_block <= 0) {
        fprintf(stderr, "buddy_new: size and min_block must be greater than zero\n");
        return NULL;
    }
    if (min_block > size) {
        fprintf(stderr, "buddy_new: min_block must be minor than size\n");
        return NULL;
    }
    size = next_pow2(size);
    min_block = next_pow2(min_block);
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
    size = next_pow2(size);
    min_block = next_pow2(min_block);
    size_t number_of_blocks = size / min_block;
    size_t bitmap_size = ceil_div(2 * number_of_blocks - 1, CHAR_BIT);
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
    size_t target_order = bitmaptree_size_to_order(allocator->bitmap, size);
    ptrdiff_t index = buddy_alloc_r(allocator, 0, target_order);
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

bool buddy_is_valid_pointer(BuddyAllocator *allocator, void* ptr) {
    if (allocator == NULL || ptr == NULL)
        return false;
    unsigned char *p = ptr;
    unsigned char *base = allocator->base;
    if (p < base || p >= base + allocator->size)
        return false;
    size_t difference = p - base;
    if (difference % allocator->min_block != 0)
        return false;
    return !bitmaptree_is_free(allocator->bitmap, pointer_to_index(allocator, ptr)) && !bitmaptree_is_split(allocator->bitmap, pointer_to_index(allocator, ptr));
}

ptrdiff_t buddy_alloc_r(BuddyAllocator *allocator, size_t node_index, size_t target_order) {
    size_t order = bitmaptree_index_to_order(allocator->bitmap, node_index);
    if (order < target_order)
        return -1;
    if (!bitmaptree_is_free(allocator->bitmap, node_index) && !bitmaptree_is_split(allocator->bitmap, node_index))
        return -1;
    if (bitmaptree_is_split(allocator->bitmap, node_index)) {
        size_t left_child_index = bitmaptree_left_child(node_index);
        size_t right_child_index = bitmaptree_right_child(node_index);
        ptrdiff_t free_index = buddy_alloc_r(allocator, left_child_index, target_order);
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
        size_t left_child_index = bitmaptree_left_child(node_index);
        return buddy_alloc_r(allocator, left_child_index, target_order);
    }
    return -1;
}

void buddy_free_r(BuddyAllocator *allocator, size_t index) {
    bitmaptree_mark_free(allocator->bitmap, index);
    if (!bitmaptree_is_root(index)) {
        size_t buddy_index = bitmaptree_buddy(index);
        if (bitmaptree_is_free(allocator->bitmap, buddy_index)) {
            size_t parent_index = bitmaptree_parent(index);
            buddy_free_r(allocator, parent_index);
        }
    }
}

size_t pointer_to_index(BuddyAllocator *allocator, void* ptr) {
    size_t difference = (unsigned char*) ptr - (unsigned char*) allocator->base;
    size_t base_index = allocator->size / allocator->min_block - 1;
    size_t leaf_block_index = difference / allocator->min_block;
    size_t index = base_index + leaf_block_index;
    while (index > 0 && (bitmaptree_is_free(allocator->bitmap, index) || bitmaptree_is_split(allocator->bitmap, index))) {
        index = bitmaptree_parent(index);
    }
    return index;
}

void* index_to_pointer(BuddyAllocator *allocator, size_t index) {
    size_t level = bitmaptree_index_to_level(index);
    size_t number_of_blocks_at_level = 1 << level;
    size_t block_size_at_level = allocator->size / number_of_blocks_at_level;
    size_t block_index_at_level = index - ((1 << level) - 1);
    return (unsigned char*) allocator->base + block_index_at_level * block_size_at_level;
}
