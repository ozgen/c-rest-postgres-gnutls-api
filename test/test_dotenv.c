//
// Created by Mehmet Ozgen on 11.01.2025.
//

#include <stdio.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "dotenv.h"

Describe(Dotenv);
BeforeEach(Dotenv) {}
AfterEach(Dotenv) {}

// Stub for setenv
int setenv_stub(const char *name, const char *value, int overwrite) {
    printf("DEBUG: setenv_stub called with name='%s', value='%s', overwrite=%d\n", name, value, overwrite);
    return (int)mock(name, value, overwrite);
}

static void expect_setenv(const char *expected_name, const char *expected_value, int expected_overwrite) {
    expect(setenv_stub,
        when(name, is_equal_to_string(expected_name)),
        when(value, is_equal_to_string(expected_value)),
        when(overwrite, is_equal_to(expected_overwrite)),
        will_return(0));
}

// Helper function to create a temporary file with test data
char *create_temp_file(const char *content) {
    char *filename = tmpnam(NULL); // Generate a temporary filename
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to create temp file");
        return NULL;
    }
    fprintf(file, "%s", content);
    fclose(file);
    return filename;
}

// Test case: File cannot be opened
Ensure(Dotenv, dotenv_load_returns_negative_when_file_cannot_be_opened) {
    int result = dotenv_load_with_dependencies("nonexistent_file.env", fopen, fgets, setenv_stub);
    assert_that(result, is_equal_to(-1));
}

// Test case: Invalid line in the .env file
Ensure(Dotenv, dotenv_load_skips_invalid_lines) {
    const char *content = "INVALID_LINE\n";
    char *filename = create_temp_file(content);

    int result = dotenv_load_with_dependencies(filename, fopen, fgets, setenv_stub);
    assert_that(result, is_equal_to(0));

    remove(filename); // Clean up
}

// Test case: Successfully sets environment variables
Ensure(Dotenv, dotenv_load_sets_environment_variables) {
    const char *content = "KEY=VALUE\n";
    char *filename = create_temp_file(content);

    expect_setenv("KEY", "VALUE", 1);
    int result = dotenv_load_with_dependencies(filename, fopen, fgets, setenv_stub);
    assert_that(result, is_equal_to(0));

    remove(filename); // Clean up
}

// Test case: Handles comments and empty lines
Ensure(Dotenv, dotenv_load_handles_comments_and_empty_lines) {
    const char *content = "# This is a comment\n\nKEY=VALUE\n";
    char *filename = create_temp_file(content);

    expect_setenv("KEY", "VALUE", 1);
    int result = dotenv_load_with_dependencies(filename, fopen, fgets, setenv_stub);
    assert_that(result, is_equal_to(0));

    remove(filename); // Clean up
}

// Group all Dotenv tests into a test suite
TestSuite *Dotenv_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Dotenv, dotenv_load_returns_negative_when_file_cannot_be_opened);
    add_test_with_context(suite, Dotenv, dotenv_load_skips_invalid_lines);
    add_test_with_context(suite, Dotenv, dotenv_load_sets_environment_variables);
    add_test_with_context(suite, Dotenv, dotenv_load_handles_comments_and_empty_lines);
    return suite;
}
