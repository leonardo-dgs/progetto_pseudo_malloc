#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tests.h"
#include "shell.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Type '%s help' for usage information.\n", argv[0]);
        return 1;
    }
    const char *cmd = argv[1];
    if (strcmp(cmd, "help") == 0) {
        printf("Available commands:\n");
        printf("  test - Run the test suite\n");
        printf("  shell [<buddy_size> <min_block>] - Start the interactive shell\n");
        printf("  help - Show this help message\n");
        return 0;
    } else if (strcmp(cmd, "test") == 0) {
        run_tests();
        return 0;
    } else if (strcmp(cmd, "shell") == 0) {
        size_t buddy_size = 1024 * 1024;
        size_t min_block = 32;
        if (argc == 3) {
            printf("Usage: %s shell [<buddy_size> <min_block>]\n", argv[0]);
            return 1;
        } else if (argc >= 4) {
            buddy_size = strtoul(argv[2], NULL, 10);
            min_block = strtoul(argv[3], NULL, 10);
        }
        run_shell(buddy_size, min_block);
        return 0;
    }
    printf("Unknown argument.\n");
    printf("Type '%s help' for usage information.\n", argv[0]);
    return 1;
}
