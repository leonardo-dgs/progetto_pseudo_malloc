#include "buddy_allocator.h"

#define BUDDY_SIZE (1024 * 1024)
#define BUDDY_BLOCK 16

typedef struct {
    size_t size;
} BlockHeader;

void *pseudo_malloc(size_t size);
void pseudo_free(void *pointer);
