#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>

#include "bitmap_tree.h"

int get_bit(unsigned char *base, size_t bit_index);
void set_bit(unsigned char *base, size_t bit_index);
void clear_bit(unsigned char *base, size_t bit_index);

BitmapTree* bitmaptree_new(size_t size, size_t min_block) {
    BitmapTree* bitmap_tree = mmap(NULL, sizeof(BitmapTree), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    bitmaptree_init(bitmap_tree, size, min_block);
    return bitmap_tree;
}

void bitmaptree_delete(BitmapTree* tree) {
    munmap(tree->bitmap_used, tree->size);
    munmap(tree->bitmap_split, tree->size);
    munmap(tree, sizeof(BitmapTree));
}

void bitmaptree_init(BitmapTree *tree, size_t size, size_t min_block) {
    tree->bitmap_used = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    tree->bitmap_split = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memset(tree->bitmap_used, 0, size);
    memset(tree->bitmap_split, 0, size);
    tree->size = size;
    tree->min_block = min_block;
}

int bitmaptree_is_split(BitmapTree *tree, size_t index) {
    return get_bit(tree->bitmap_split, index);
}

void bitmaptree_mark_split(BitmapTree *tree, size_t index) {
    clear_bit(tree->bitmap_used, index);
    set_bit(tree->bitmap_split, index);
}

int bitmaptree_is_free(BitmapTree *tree, size_t index) {
    return !get_bit(tree->bitmap_used, index) && !get_bit(tree->bitmap_split, index);
}

void bitmaptree_mark_used(BitmapTree *tree, size_t index) {
    set_bit(tree->bitmap_used, index);
    clear_bit(tree->bitmap_split, index);
}

void bitmaptree_mark_free(BitmapTree *tree, size_t index) {
    clear_bit(tree->bitmap_used, index);
    clear_bit(tree->bitmap_split, index);
}

int bitmaptree_parent(BitmapTree *tree, size_t index) {
    return (index - 1) / 2;
}

int bitmaptree_left_child(BitmapTree *tree, size_t index) {
    return 2 * index + 1;
}

int bitmaptree_right_child(BitmapTree *tree, size_t index) {
    return 2 * index + 2;
}

int bitmaptree_buddy(BitmapTree *tree, size_t index) {
    return (index % 2 == 0) ? index - 1 : index + 1;
}

int bitmaptree_size_to_order(BitmapTree *tree, size_t size) {
    int result = ceil(log2(ceil((double) size / (double) tree->min_block)));
    return result;
}

int bitmaptree_index_to_order(BitmapTree *tree, size_t index) {
    int number_of_nodes = tree->size * 8;
    int number_of_blocks = (number_of_nodes + 1) / 2;
    int max_order = (int) floor(log2(number_of_blocks));
    int level = bitmaptree_index_to_level(tree, index);
    return max_order - level;
}

int bitmaptree_index_to_level(BitmapTree *tree, size_t index) {
    return floor(log2(index + 1));
}

int bitmaptree_is_leaf(BitmapTree *tree, size_t index) {
    return bitmaptree_index_to_order(tree, index) == 0;
}

int bitmaptree_is_root(size_t index) {
    return index == 0;
}

int get_bit(unsigned char *base, size_t bit_index) {
    size_t byte_index = bit_index / 8;
    size_t offset = bit_index % 8;
    unsigned char bit_mask = 1 << offset;
    return (base[byte_index] & bit_mask) != 0;
}

void set_bit(unsigned char *base, size_t bit_index) {
    size_t byte_index = bit_index / 8;
    size_t offset = bit_index % 8;
    unsigned char bit_mask = 1 << offset;
    base[byte_index] |= bit_mask;
}

void clear_bit(unsigned char *base, size_t bit_index) {
    size_t byte_index = bit_index / 8;
    size_t offset = bit_index % 8;
    unsigned char bit_mask = ~(1 << offset);
    base[byte_index] &= bit_mask;
}
