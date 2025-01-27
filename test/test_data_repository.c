#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "data_repository.h"

// Stub functions
static PGconn *PQconnectdb_stub(const char *conninfo) {
    return (PGconn *)mock(conninfo);
}

static ConnStatusType PQstatus_stub(const PGconn *conn) {
    return (ConnStatusType)mock(conn);
}

static PGresult *PQexec_stub(PGconn *conn, const char *query) {
    return (PGresult *)mock(conn, query);
}

static ExecStatusType PQresultStatus_stub(const PGresult *res) {
    return (ExecStatusType)mock(res);
}

static int PQntuples_stub(const PGresult *res) {
    return (int)(intptr_t)mock(res);
}

static char *PQgetvalue_stub(const PGresult *res, int row, int col) {
    return (char *)mock(res, row, col);
}

static void PQclear_stub(PGresult *res) {
    mock(res);
}

static void PQfinish_stub(PGconn *conn) {
    mock(conn);
}

// Test case using stubs
Describe(DataRepository);
BeforeEach(DataRepository) {}
AfterEach(DataRepository) {}

Ensure(DataRepository, fetch_data_returns_mocked_data) {
    // Setup expectations for mocked functions
    const char *connection_str = "host=localhost port=5432 dbname=testdb user=test password=test";
    const char *query_str = "SELECT data FROM sample_table LIMIT 1;";

    PGconn *mock_conn = (PGconn *)0x1234;
    PGresult *mock_result = (PGresult *)0x5678;

    expect(PQconnectdb_stub, when(conninfo, is_equal_to_string(connection_str)),
           will_return(mock_conn));

    expect(PQstatus_stub, when(conn, is_equal_to(mock_conn)),
           will_return(CONNECTION_OK));

    expect(PQexec_stub, when(conn, is_equal_to(mock_conn)),
           when(query, is_equal_to_string(query_str)),
           will_return(mock_result));

    expect(PQresultStatus_stub, when(res, is_equal_to(mock_result)),
           will_return(PGRES_TUPLES_OK));

    expect(PQntuples_stub, when(res, is_equal_to(mock_result)),
           will_return(1));

    expect(PQgetvalue_stub, when(res, is_equal_to(mock_result)),
           when(row, is_equal_to(0)),
           when(col, is_equal_to(0)),
           will_return("Mocked Data"));

    expect(PQclear_stub, when(res, is_equal_to(mock_result)));
    expect(PQfinish_stub, when(conn, is_equal_to(mock_conn)));

    // Inject stubs into the function under test
    const char *result = fetch_data_with_stubs(
        &PQconnectdb_stub,
        &PQstatus_stub,
        &PQexec_stub,
        &PQresultStatus_stub,
        &PQntuples_stub,
        &PQgetvalue_stub,
        &PQclear_stub,
        &PQfinish_stub
    );

    // Verify the result
    assert_that(result, is_equal_to_string("Mocked Data"));
}

// Function to return the DataRepository test suite
TestSuite *DataRepository_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, DataRepository, fetch_data_returns_mocked_data);
    return suite;
}

int main(int argc, char **argv) {
    TestSuite *suite = create_test_suite();
    add_suite(suite, DataRepository_tests());
    return run_test_suite(suite, create_text_reporter());
}
