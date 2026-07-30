#include <stdio.h>
#include <string.h>

#include "tests.h"
#include "shell.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <test|shell>\n", argv[0]);
        return 1;
    }
    const char *cmd = argv[1];
    if (strcmp(cmd, "test") == 0) {
        run_tests();
        return 0;
    } else if (strcmp(cmd, "shell") == 0) {
        run_shell();
        return 0;
    }
    printf("Unknown argument.\n");
    printf("Usage: %s <test|shell>\n", argv[0]);
    return 1;
}
