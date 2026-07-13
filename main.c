#include <stdio.h>
#include <string.h>

#include "tests.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You need to supply an argument.\n");
    }
    char* cmd = argv[1];
    if (strcmp(cmd, "test")) {
        run_tests();
        return;
    }
    printf("Unknown command.\n");
}
