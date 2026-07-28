#ifndef BITMAP_TREE_H
#define BITMAP_TREE_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    unsigned char *bitmap_used;
    unsigned char *bitmap_split;
    size_t size;
    size_t min_block;
} BitmapTree;

BitmapTree* bitmaptree_new(size_t size, size_t min_block);
void bitmaptree_delete(BitmapTree* tree);

void bitmaptree_init(BitmapTree *tree, size_t size, size_t min_block);

bool bitmaptree_is_split(BitmapTree *tree, size_t index);
void bitmaptree_mark_split(BitmapTree *tree, size_t index);

bool bitmaptree_is_free(BitmapTree *tree, size_t index);
void bitmaptree_mark_used(BitmapTree *tree, size_t index);
void bitmaptree_mark_free(BitmapTree *tree, size_t index);

size_t bitmaptree_parent(size_t index);

size_t bitmaptree_left_child(size_t index);
size_t bitmaptree_right_child(size_t index);

size_t bitmaptree_buddy(size_t index);

size_t bitmaptree_number_of_nodes(BitmapTree *tree);
size_t bitmaptree_number_of_leaves(BitmapTree *tree);

size_t bitmaptree_size_to_order(BitmapTree *tree, size_t size);
size_t bitmaptree_index_to_order(BitmapTree *tree, size_t index);
size_t bitmaptree_index_to_level(size_t index);

bool bitmaptree_is_leaf(BitmapTree *tree, size_t index);
bool bitmaptree_is_root(size_t index);

#endif