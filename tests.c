#include <stdio.h>
#include <assert.h>

#include "pseudo_malloc.h"
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

    for (int i = 0; i < 100; i++)
        assert(p3[i] == i);
}
