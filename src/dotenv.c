//
// Created by Mehmet Ozgen on 10.01.2025.
//

#include "dotenv.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int dotenv_load_with_dependencies(const char *filename,
                                   FILE *(*fopen_func)(const char *, const char *),
                                   char *(*fgets_func)(char *, int, FILE *),
                                   int (*setenv_func)(const char *, const char *, int)) {
    FILE *file = fopen_func(filename, "r");
    if (!file) {
        perror("Failed to open .env file");
        return -1;
    }

    char line[1024];
    while (fgets_func(line, sizeof(line), file)) {
        // Ignore comments and empty lines
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        // Find '=' character
        char *delimiter = strchr(line, '=');
        if (!delimiter) {
            fprintf(stderr, "Invalid line in .env file: %s\n", line);
            continue;
        }

        // Split into key and value
        *delimiter = '\0';
        const char *key = line;
        const char *value = delimiter + 1;

        printf("DEBUG: Key = '%s', Value = '%s'\n", key, value); // Debugging output

        // Set the environment variable
        if (setenv_func(key, value, 1) != 0) {
            perror("Failed to set environment variable");
        }
    }

    fclose(file);
    return 0;
}


// Default implementation using real functions
int dotenv_load(const char *filename) {
    return dotenv_load_with_dependencies(filename, fopen, fgets, setenv);
}
