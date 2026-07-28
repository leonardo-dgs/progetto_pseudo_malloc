#include <stdio.h>
#include <string.h>

#include "tests.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You need to supply an argument.\n");
        return 0;
    }
    const char *cmd = argv[1];
    if (strcmp(cmd, "test") == 0) {
        run_tests();
        return 0;
    }
    printf("Unknown argument.\n");
}
