// Created by moezgen on 1/16/25.

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "db_connection.h"

// Mocked functions
static PGconn *mock_connect_func(const char *conninfo) {
    return (PGconn *)mock(conninfo);
}

static ConnStatusType mock_status_func(const PGconn *conn) {
    return (ConnStatusType)mock(conn);
}

static void mock_finish_func(PGconn *conn) {
    mock(conn);
}

static const char *mock_error_func(const PGconn *conn) {
    return (const char *)mock(conn);
}

// Test case setup
Describe(DBConnection);

BeforeEach(DBConnection) {
    // Set environment variables for the tests
    setenv("POSTGRES_HOST", "localhost", 1);
    setenv("POSTGRES_PORT", "5432", 1);
    setenv("POSTGRES_DB", "testdb", 1);
    setenv("POSTGRES_USER", "test", 1);
    setenv("POSTGRES_PASSWORD", "test", 1);
}

AfterEach(DBConnection) {
    // Unset environment variables after each test
    unsetenv("POSTGRES_HOST");
    unsetenv("POSTGRES_PORT");
    unsetenv("POSTGRES_DB");
    unsetenv("POSTGRES_USER");
    unsetenv("POSTGRES_PASSWORD");
}

Ensure(DBConnection, create_connection_returns_valid_connection) {
    PGconn *mock_conn = (PGconn *)0x1234;

    DBHelpers helpers = {
        .connect_func = mock_connect_func,
        .status_func = mock_status_func,
        .finish_func = mock_finish_func,
        .error_func = mock_error_func
    };

    // Set up expectations
    expect(mock_connect_func,
           when(conninfo, is_equal_to_string("host=localhost port=5432 dbname=testdb user=test password=test")),
           will_return(mock_conn));

    expect(mock_status_func,
           when(conn, is_equal_to(mock_conn)),
           will_return(CONNECTION_OK));

    // Call the function
    char *conninfo = build_connection_string();
    PGconn *conn = create_connection(conninfo, &helpers);
    free(conninfo);

    // Verify the result
    assert_that(conn, is_equal_to(mock_conn));
}

Ensure(DBConnection, create_connection_handles_failed_connection) {
    PGconn *mock_conn = (PGconn *)0x1234;

    DBHelpers helpers = {
        .connect_func = mock_connect_func,
        .status_func = mock_status_func,
        .finish_func = mock_finish_func,
        .error_func = mock_error_func
    };

    // Set up expectations for a failed connection
    expect(mock_connect_func,
           when(conninfo, is_equal_to_string("host=localhost port=5432 dbname=testdb user=test password=test")),
           will_return(mock_conn));

    expect(mock_status_func,
           when(conn, is_equal_to(mock_conn)),
           will_return(CONNECTION_BAD));

    expect(mock_error_func,
           when(conn, is_equal_to(mock_conn)),
           will_return("(mocked error message)"));

    expect(mock_finish_func,
           when(conn, is_equal_to(mock_conn)));

    // Call the function
    char *conninfo = build_connection_string();
    PGconn *conn = create_connection(conninfo, &helpers);
    free(conninfo);

    // Verify the result
    assert_that(conn, is_null);
}

Ensure(DBConnection, close_connection_closes_valid_connection) {
    PGconn *mock_conn = (PGconn *)0x1234;

    DBHelpers helpers = {
        .connect_func = mock_connect_func,
        .status_func = mock_status_func,
        .finish_func = mock_finish_func,
        .error_func = mock_error_func
    };

    // Set up expectation for closing connection
    expect(mock_finish_func, when(conn, is_equal_to(mock_conn)));

    // Call the function
    close_connection(mock_conn, &helpers);
}

Ensure(DBConnection, close_connection_handles_null_connection) {
    DBHelpers helpers = {
        .connect_func = mock_connect_func,
        .status_func = mock_status_func,
        .finish_func = mock_finish_func,
        .error_func = mock_error_func
    };

    // Call the function with NULL
    close_connection(NULL, &helpers);

    // No expectations since NULL is handled gracefully
}

// Function to return the DBConnection test suite
TestSuite *DBConnection_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, DBConnection, create_connection_returns_valid_connection);
    add_test_with_context(suite, DBConnection, create_connection_handles_failed_connection);
    add_test_with_context(suite, DBConnection, close_connection_closes_valid_connection);
    add_test_with_context(suite, DBConnection, close_connection_handles_null_connection);
    return suite;
}

int main(int argc, char **argv) {
    TestSuite *suite = create_test_suite();
    add_suite(suite, DBConnection_tests());
    return run_test_suite(suite, create_text_reporter());
}
