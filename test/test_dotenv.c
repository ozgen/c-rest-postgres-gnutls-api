//
// Created by Mehmet Ozgen on 11.01.2025.
//
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dotenv.h"

Describe(Dotenv);
BeforeEach(Dotenv) {}
AfterEach(Dotenv) {}

// Stub for fopen
FILE *fopen_stub(const char *filename, const char *mode) {
    return (FILE *)mock(filename, mode);
}

// Stub for fgets
char *fgets_stub(char *str, int num, FILE *stream) {
    char *mock_line = (char *)mock(stream);
    if (mock_line) {
        strncpy(str, mock_line, num - 1); // Copy mocked line
        str[num - 1] = '\0';             // Ensure null termination
        return str;
    }
    return NULL; // Simulate EOF
}

// Stub for setenv
int setenv_stub(const char *name, const char *value, int overwrite) {
    return (int)mock(name, value, overwrite);
}

static void expect_setenv(const char *expected_name, const char *expected_value, int expected_overwrite) {
    expect(setenv_stub,
        when(name, is_equal_to_string(expected_name)),
        when(value, is_equal_to_string(expected_value)),
        when(overwrite, is_equal_to(expected_overwrite)),
        will_return(0)); // Simulate successful environment variable setting
}

// Test case: File cannot be opened
Ensure(Dotenv, dotenv_load_returns_negative_when_file_cannot_be_opened) {
    expect(fopen_stub, when(filename, is_equal_to(".env")), will_return(NULL));

    int result = dotenv_load_with_dependencies(".env", fopen_stub, fgets_stub, setenv_stub);

    assert_that(result, is_equal_to(-1));
}

// Test case: Invalid line in the .env file
Ensure(Dotenv, dotenv_load_skips_invalid_lines) {
    FILE fake_file;

    expect(fopen_stub, when(filename, is_equal_to(".env")), will_return(&fake_file));
    expect(fgets_stub, when(stream, is_equal_to(&fake_file)), will_return("INVALID_LINE"));
    expect(fgets_stub, will_return(NULL)); // Simulate EOF

    int result = dotenv_load_with_dependencies(".env", fopen_stub, fgets_stub, setenv_stub);

    assert_that(result, is_equal_to(0));
}

// Test case: Successfully sets environment variables
Ensure(Dotenv, dotenv_load_sets_environment_variables) {
    FILE fake_file;

    // Simulate reading from .env file
    expect(fopen_stub, when(filename, is_equal_to(".env")), will_return(&fake_file));
    expect(fgets_stub, when(stream, is_equal_to(&fake_file)), will_return("KEY=VALUE"));
    expect(fgets_stub, will_return(NULL)); // Simulate EOF

    // Set expectations for setenv_stub calls
    expect_setenv("KEY", "VALUE", 1);

    // Call the function under test
    int result = dotenv_load_with_dependencies(".env", fopen_stub, fgets_stub, setenv_stub);

    // Verify the result
    assert_that(result, is_equal_to(0));
}


// Group all Dotenv tests into a test suite
TestSuite *Dotenv_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Dotenv, dotenv_load_returns_negative_when_file_cannot_be_opened);
    add_test_with_context(suite, Dotenv, dotenv_load_skips_invalid_lines);
    add_test_with_context(suite, Dotenv, dotenv_load_sets_environment_variables);
    return suite;
}
