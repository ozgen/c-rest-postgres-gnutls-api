//
// Created by Mehmet Ozgen on 10.01.2025.
//

#ifndef DOTENV_H
#define DOTENV_H
#include <stdio.h>

int dotenv_load(const char *filename);

int dotenv_load_with_dependencies(
    const char *filename,
    FILE *(*fopen_func)(const char *, const char *),
    char *(*fgets_func)(char *, int, FILE *),
    int (*setenv_func)(const char *, const char *, int));

#endif // DOTENV_H

