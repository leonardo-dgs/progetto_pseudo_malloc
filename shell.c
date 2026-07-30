#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "buddy_allocator.h"

#include "shell.h"

bool execute_command(BuddyAllocator *buddy_allocator, size_t argc, char **argv);

void run_shell(size_t buddy_size, size_t min_block) {
    printf("Welcome to the buddy shell!\n");
    printf("Type 'help' for a list of commands.\n");
    BuddyAllocator *buddy_allocator = buddy_new(buddy_size, min_block);
    if (buddy_allocator == NULL) {
        fprintf(stderr, "Failed to initialize buddy allocator.\n");
        return;
    }
    bool running = true;
    while (running) {
        char input[256];
        printf("\nbuddy> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        char *token = strtok(input, " \n");
        char *args[256];
        size_t argc = 0;
        while (token != NULL) {
            args[argc++] = token;
            token = strtok(NULL, " \n");
        }
        running = execute_command(buddy_allocator, argc, args);
    }
    buddy_delete(buddy_allocator);
}

bool execute_command(BuddyAllocator *buddy_allocator, size_t argc, char **argv) {
    if (argc == 0) {
        return true;
    }
    char *command = argv[0];
    if (strcmp(command, "help") == 0) {
        printf("Available commands:\n\n");
        printf("  help - Show this help message\n");
        printf("  alloc <size> - Allocate memory of the specified size\n");
        printf("  free <pointer> - Free the memory at the specified pointer\n");
        printf("  dump - Display the current state of the allocator\n");
        printf("  stats - Display statistics about the allocator\n");
        printf("  exit - Exit the shell\n");
    } else if(strcmp(command, "alloc") == 0) {
        if (argc < 2) {
            printf("Usage: alloc <size>\n");
            return true;
        }
        size_t size = strtoul(argv[1], NULL, 10);
        void *ptr = buddy_alloc(buddy_allocator, size);
        if (ptr == NULL) {
            printf("Allocation failed for size: %zu\n", size);
            return true;
        }
        printf("Allocated %zu bytes at pointer: %p\n", size, ptr);
    } else if (strcmp(command, "free") == 0) {
        if (argc < 2) {
            printf("Usage: free <pointer>\n");
            return true;
        }
        void *ptr = (void *) strtoul(argv[1], NULL, 16);
        if (!buddy_is_valid_pointer(buddy_allocator, ptr)) {
            printf("Invalid pointer: %p\n", ptr);
            return true;
        }
        buddy_free(buddy_allocator, ptr);
        printf("Free command executed.\n");
    } else if (strcmp(command, "dump") == 0) {
        printf("Buddy Allocator Tree Dump:\n\n");
        buddy_dump_tree(buddy_allocator);
    } else if (strcmp(command, "stats") == 0) {
        printf("Buddy Allocator Stats:\n\n");
        buddy_print_stats(buddy_allocator);
    } else if (strcmp(command, "exit") == 0) {
        return false;
    } else {
        printf("Unknown command: %s\n", command);
    }
    return true;
}
