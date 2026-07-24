#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include "bitmap_tree.h"
#include "tests.h"

void run_tests() {
    void* p1 = pseudo_malloc(1);
    assert(p1 != NULL);
    void* p2 = pseudo_malloc(0);
    assert(p2 == NULL);

    char *p3 = pseudo_malloc(100);
    assert(p3 != NULL);
    for (int i = 0; i < 100;  i++)
        p3[i] = (char) i;

    pseudo_free(p1);
    pseudo_free(NULL);

    char *p4 = pseudo_malloc(20);
    assert(p4 != NULL);

    memset(p4, 0xAA, 20);

    for (int i = 0; i < 100; i++)
        assert(p3[i] == i);

    pseudo_free(p2);
    pseudo_free(p3);
    pseudo_free(p4);

    void *a = pseudo_malloc(64);
    assert(a != NULL);

    pseudo_free(a);

    void *b = pseudo_malloc(64);
    assert(b != NULL);

    void *large = pseudo_malloc(1024 * 1024);
    assert(large != NULL);

    memset(large, 0xAA, 1024 * 1024);

    pseudo_free(large);


    BuddyAllocator* buddy_allocator = buddy_new(512 * 512, 32);
    assert(buddy_allocator != NULL);

    void* b1 = buddy_alloc(buddy_allocator, 11);
    assert(b1 != NULL);
    assert(buddy_is_valid_pointer(buddy_allocator, b1));
    void* b2 = buddy_alloc(buddy_allocator, 0);
    assert(b2 == NULL);
    assert(!buddy_is_valid_pointer(buddy_allocator, b2));

    char *b3 = buddy_alloc(buddy_allocator, 100);
    assert(buddy_is_valid_pointer(buddy_allocator, b3));
    for (int i = 0; i < 100;  i++)
        b3[i] = (char) i;

    buddy_free(buddy_allocator, b1);
    buddy_free(buddy_allocator, NULL);

    char *b4 = buddy_alloc(buddy_allocator, 20);
    assert(buddy_is_valid_pointer(buddy_allocator, b4));

    for (int i = 0; i < 100; i++)
        assert(b3[i] == i);

    buddy_free(buddy_allocator, b2);
    buddy_free(buddy_allocator, b3);
    buddy_free(buddy_allocator, b4);

    void *ptrs[100];
    for (int i = 0; i < 100; i++) {
        ptrs[i] = buddy_alloc(buddy_allocator, 32);
        assert(buddy_is_valid_pointer(buddy_allocator, ptrs[i]));
    }

    for (int i = 0; i < 100; i++)
        buddy_free(buddy_allocator, ptrs[i]);

    void *x1 = buddy_alloc(buddy_allocator, 64);
    void *x2 = buddy_alloc(buddy_allocator, 64);

    assert(buddy_is_valid_pointer(buddy_allocator, x1));
    assert(buddy_is_valid_pointer(buddy_allocator, x2));

    buddy_free(buddy_allocator, x1);
    buddy_free(buddy_allocator, x2);

    void *big = buddy_alloc(buddy_allocator, 256 * 256);
    assert(buddy_is_valid_pointer(buddy_allocator, big));

    buddy_free(buddy_allocator, big);

    void *huge = buddy_alloc(buddy_allocator, 512 * 512);
    assert(buddy_is_valid_pointer(buddy_allocator, huge));
    buddy_free(buddy_allocator, huge);
}
