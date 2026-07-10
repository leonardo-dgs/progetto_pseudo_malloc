#include <stdlib.h>

#include "bitmap_tree.h"

void bitmaptree_init(BitmapTree *tree, size_t size, size_t min_block) {
    tree->bitmap_used = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    tree->bitmap_split = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memset(tree->bitmap_used, 0, size);
    memset(tree->bitmap_split, 0, size);
    tree->size = size;
    tree->min_block = min_block;
}

int bitmaptree_is_split(BitmapTree *tree, int index) {
    return tree->bitmap_split[index];
}

void bitmaptree_mark_split(BitmapTree *tree, int index) {
    tree->bitmap_used[index] = 0;
    tree->bitmap_split[index] = 1;
}

int bitmaptree_is_free(BitmapTree *tree, int index) {
    return !tree->bitmap_used[index] && !tree->bitmap_split[index];
}

void bitmaptree_mark_used(BitmapTree *tree, int index) {
    tree->bitmap_used[index] = 1;
    tree->bitmap_split[index] = 0;
}

void bitmaptree_mark_free(BitmapTree *tree, int index) {
    tree->bitmap_used[index] = 0;
    tree->bitmap_split[index] = 0;
}

int bitmaptree_parent(BitmapTree *tree, int index) {
    return (index - 1) / 2;
}

int bitmaptree_left_child(BitmapTree *tree, int index) {
    return 2 * index + 1;
}

int bitmaptree_right_child(BitmapTree *tree, int index) {
    return 2 * index + 2;
}

int bitmaptree_buddy(BitmapTree *tree, int index) {
    return (i % 2 == 0) ? index - 1 : index + 1;
}

int bitmaptree_size_to_order(BitmapTree *tree, size_t size) {
    return (int) ceil(log2(size / tree->min_block));
}

int bitmaptree_index_to_order(BitmapTree *tree, size_t index) {
    int number_of_blocks = tree->size * 8;
    int max_order = (int) floor(log2(number_of_blocks));
    int level = (int) floor(log2(index + 1));
    return max_order - level;
}

int bitmaptree_is_leaf(BitmapTree *tree, int index) {
    return index_to_order(allocator, index) == 0;
}

int bitmaptree_is_root(int index) {
    return index == 0;
}
