//
// Created by moezgen on 1/13/25.
//
#include <assert.h>
#include <dirent.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "migration_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct MockPGresult {
    ExecStatusType status;
    int ntuples;
} MockPGresult;

// Stub and Mock Functions
static PGresult *mock_exec_func(PGconn *conn, const char *query) {
    assert(conn != NULL);
    assert(query != NULL);

    MockPGresult *mock_result = (MockPGresult *)malloc(sizeof(MockPGresult));
    if (strstr(query, "CREATE TABLE")) {
        printf("DEBUG: mock_exec_func returns PGRES_COMMAND_OK\n");
        mock_result->status = PGRES_COMMAND_OK;
        mock_result->ntuples = 0;
    } else {
        printf("DEBUG: mock_exec_func returns PGRES_FATAL_ERROR\n");
        mock_result->status = PGRES_FATAL_ERROR;
        mock_result->ntuples = 0;
    }
    return (PGresult *)mock_result;
}

static void mock_clear_func(PGresult *res) {
    printf("DEBUG: mock_clear_func called\n");
    assert(res != NULL);
    free(res); // Free the mock result memory
}

static ExecStatusType mock_result_status_func(const PGresult *res) {
    printf("DEBUG: mock_result_status_func called\n");
    assert(res != NULL);
    const MockPGresult *mock_result = (const MockPGresult *)res;
    return mock_result->status;
}

static void mock_finish_func(PGconn *conn) {
    printf("DEBUG: mock_finish_func called with connection: %p\n", conn);
    assert(conn != NULL);
    mock(conn);
}

static void mock_apply_migration_func(PGconn *conn, const char *filepath, DBHelpers *db_helpers) {
    printf("DEBUG: mock_apply_migration_func called with filepath: %s\n", filepath ? filepath : "NULL");
    printf("DEBUG: mock_apply_migration_func called with connection: %p\n", conn);
    assert(conn != NULL);
    assert(filepath != NULL);
    assert(db_helpers != NULL);
    mock(conn, filepath, db_helpers);
}

// Helper Function to Create Temporary SQL File
static char *create_sql_temp_file(const char *content) {
    char filename[] = "/tmp/testfileXXXXXX";
    int fd = mkstemp(filename); // Use mkstemp for secure temporary file creation
    if (fd == -1) {
        perror("Failed to create temp file");
        return NULL;
    }
    FILE *file = fdopen(fd, "w");
    if (!file) {
        perror("Failed to open temp file");
        close(fd);
        return NULL;
    }
    fprintf(file, "%s", content);
    fclose(file);
    printf("DEBUG: Temporary SQL file created: %s\n", filename);
    return strdup(filename);
}

// Test case setup
Describe(MigrationManager_Migrations);

BeforeEach(MigrationManager_Migrations) {
}

AfterEach(MigrationManager_Migrations) {
}

Ensure(MigrationManager_Migrations, run_migrations_executes_correctly_with_temp_file) {
    printf("DEBUG: Starting run_migrations_executes_correctly_with_temp_file\n");

    PGconn *mock_connection = (PGconn *)1; // Mock PGconn object
    const char *mock_migrations_path = "/tmp/test_migrations";

    // Create the migrations directory
    mkdir(mock_migrations_path, 0700);

    // Create a temporary SQL file
    const char *mock_migration_content = "CREATE TABLE test_table (id SERIAL PRIMARY KEY);";
    char *temp_file = create_sql_temp_file(mock_migration_content);
    assert_that(temp_file, is_not_null);

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/migration.sql", mock_migrations_path);
    rename(temp_file, filepath);

    // Initialize DBHelpers struct
    DBHelpers mock_db_helpers = {
        .exec_func = mock_exec_func,
        .clear_func = mock_clear_func,
        .get_pqResultStatus = mock_result_status_func,
        .finish_func = mock_finish_func,
    };

    // Mock initialization query
    const char *init_query =
            "CREATE TABLE IF NOT EXISTS schema_migrations ("
            "id SERIAL PRIMARY KEY, "
            "filename TEXT UNIQUE NOT NULL, "
            "applied_at TIMESTAMPTZ DEFAULT NOW());";

    PGresult *mock_init_result = mock_exec_func(mock_connection, init_query);

    // Mock applying migrations
    expect(mock_apply_migration_func,
           when(conn, is_equal_to(mock_connection)),
           when(filepath, contains_string(filepath)),
           when(db_helpers, is_equal_to(&mock_db_helpers)));

    // Mock finalization
    expect(mock_finish_func, when(conn, is_equal_to(mock_connection)));

    // Run the migrations
    printf("DEBUG: Running run_migrations\n");
    run_migrations(mock_connection, mock_migrations_path, &mock_db_helpers, mock_apply_migration_func);

    // Cleanup
    printf("DEBUG: Cleaning up resources\n");
    remove(filepath);
    rmdir(mock_migrations_path);
    free(temp_file);
    free(mock_init_result);

}

// Function to return the MigrationManager_Migrations test suite
TestSuite *MigrationManager_Migrations_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, MigrationManager_Migrations, run_migrations_executes_correctly_with_temp_file);
    return suite;
}
