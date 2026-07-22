#ifndef PSEUDO_MALLOC_H
#define PSEUDO_MALLOC_H

#define BUDDY_SIZE (1024 * 1024)
#define BUDDY_BLOCK 16

void *pseudo_malloc(size_t size);
void pseudo_free(void *pointer);

#endif