#define BUDDY_SIZE (1024 * 1024)
#define BUDDY_BLOCK 16

struct {
    char *bitmap;
    void *base;
    size_t size;
} buddy_alloc_t;

typedef struct {
    size_t size;
} block_header_t;

void *pseudo_malloc(size_t size);
void pseudo_free(void *pointer);
