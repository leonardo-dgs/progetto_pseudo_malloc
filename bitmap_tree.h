typedef struct {
    char *bitmap_used;
    char *bitmap_split;
    size_t size;
    size_t min_block;
} BitmapTree;

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
