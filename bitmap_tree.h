#ifndef BITMAP_TREE_H
#define BITMAP_TREE_H

typedef struct {
    char *bitmap_used;
    char *bitmap_split;
    size_t size;
    size_t min_block;
} BitmapTree;

BitmapTree* bitmaptree_new(size_t size, size_t min_block);

void bitmaptree_init(BitmapTree *tree, size_t size, size_t min_block);

int bitmaptree_is_split(BitmapTree *tree, int index);
void bitmaptree_mark_split(BitmapTree *tree, int index);

int bitmaptree_is_free(BitmapTree *tree, int index);
void bitmaptree_mark_used(BitmapTree *tree, int index);
void bitmaptree_mark_free(BitmapTree *tree, int index);

int bitmaptree_parent(BitmapTree *tree, int index);

int bitmaptree_left_child(BitmapTree *tree, int index);
int bitmaptree_right_child(BitmapTree *tree, int index);

int bitmaptree_buddy(BitmapTree *tree, int index);

int bitmaptree_size_to_order(BitmapTree *tree, size_t size);
int bitmaptree_index_to_order(BitmapTree *tree, size_t index);
int bitmaptree_is_leaf(BitmapTree *tree, int index);
int bitmaptree_is_root(int index);

#endif