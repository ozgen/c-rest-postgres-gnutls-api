//
// Created by moezgen on 1/13/25
//
#include "migration_manager.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

// Comparator function for qsort
int compare_filenames(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// Execute a single SQL query
void execute_sql(PGconn *conn, const char *query, const DBHelpers *db_helpers) {
    assert(conn != NULL);
    assert(query != NULL);
    assert(db_helpers != NULL);

    PGresult *res = db_helpers->exec_func(conn, query);

    if (res == NULL || db_helpers->get_pqResultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error executing SQL: %s\nQuery: %s\n", PQerrorMessage(conn), query);
    }
    if (res != NULL) {
        db_helpers->clear_func(res);
    }
}

// Apply a single migration file
void apply_migration(PGconn *conn, const char *filename, const DBHelpers *db_helpers) {
    assert(conn != NULL);
    assert(filename != NULL);
    assert(db_helpers != NULL);

    printf("DEBUG: Checking if migration is already applied for file: %s\n", filename);
    char check_query[1024];
    snprintf(check_query, sizeof(check_query),
             "SELECT 1 FROM schema_migrations WHERE filename = '%s';", filename);

    PGresult *res = db_helpers->exec_func(conn, check_query);
    if (res == NULL) {
        fprintf(stderr, "Error: exec_func returned NULL for query: %s\n", check_query);
        return;
    }
    if (db_helpers->get_pqResultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error checking migration status: %s\n", PQerrorMessage(conn));
        db_helpers->clear_func(res);
        return;
    }

    if (PQntuples(res) > 0) {
        printf("Skipping already applied migration: %s\n", filename);
        db_helpers->clear_func(res);
        return;
    }
    db_helpers->clear_func(res);

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open migration file: %s\n", filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    if (filesize <= 0) {
        fprintf(stderr, "Empty or invalid migration file: %s\n", filename);
        fclose(file);
        return;
    }

    char *query = (char *)malloc(filesize + 1);
    if (!query) {
        fprintf(stderr, "Memory allocation failed for file: %s\n", filename);
        fclose(file);
        return;
    }

    fread(query, 1, filesize, file);
    query[filesize] = '\0'; // Null-terminate the query
    fclose(file);

    execute_sql(conn, query, db_helpers); // Execute the migration query
    free(query);

    char record_query[1024];
    snprintf(record_query, sizeof(record_query),
             "INSERT INTO schema_migrations (filename) VALUES ('%s');", filename);
    execute_sql(conn, record_query, db_helpers);

    printf("Applied migration: %s\n", filename);
}

// Apply migrations with injected dependencies
void run_migrations(
    PGconn *conn,
    const char *migrations_path,
    const DBHelpers *db_helpers,
    void (*apply_migration_func)(PGconn *, const char *, const DBHelpers *)) {
    assert(conn != NULL);
    assert(migrations_path != NULL);
    assert(db_helpers != NULL);
    assert(apply_migration_func != NULL);

    static int init_called = 0;  // Flag to ensure init_query is executed once
    if (!init_called) {
        const char *init_query =
            "CREATE TABLE IF NOT EXISTS schema_migrations ("
            "id SERIAL PRIMARY KEY, "
            "filename TEXT UNIQUE NOT NULL, "
            "applied_at TIMESTAMPTZ DEFAULT NOW());";

        printf("DEBUG: Initializing schema migrations table\n");
        execute_sql(conn, init_query, db_helpers);
        init_called = 1;  // Set flag to prevent further calls
        printf("DEBUG: Initializing schema migrations table done with init_query:  %s\n", init_query);
    }

    printf("DEBUG: Opening migrations directory: %s\n", migrations_path);
    DIR *dir = opendir(migrations_path);
    if (!dir) {
        fprintf(stderr, "Failed to open migrations directory: %s\n", migrations_path);
        db_helpers->finish_func(conn);
        return;
    }

    char *filenames[1024];
    int count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".sql")) {
            filenames[count++] = strdup(entry->d_name);
        }
    }
    closedir(dir);

    qsort(filenames, count, sizeof(char *), compare_filenames);

    for (int i = 0; i < count; i++) {
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", migrations_path, filenames[i]);
        printf("DEBUG: Applying migration: %s\n", filepath);

        apply_migration_func(conn, filepath, db_helpers);

        free(filenames[i]);
    }

    printf("DEBUG: Finalizing migration process\n");
    db_helpers->finish_func(conn);
}

