#ifndef PSEUDO_MALLOC_H
#define PSEUDO_MALLOC_H

#include <stddef.h>

void *pseudo_malloc(size_t size);
void pseudo_free(void *pointer);

#endif