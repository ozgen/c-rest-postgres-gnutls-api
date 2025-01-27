#include "data_repository.h"
#include <stdio.h>
#include <stdlib.h>

const char *fetch_data(DBHelpers *helpers) {
    static char result[256];

    if (!helpers || !helpers->connect_func || !helpers->status_func || !helpers->finish_func) {
        fprintf(stderr, "DBConnHelpers not set up properly.\n");
        return "Database Configuration Error";
    }

    char *conninfo = build_connection_string();
    if (!conninfo) {
        return "Database Configuration Error";
    }

    PGconn *conn = helpers->connect_func(conninfo);
    free(conninfo);

    if (helpers->status_func(conn) != CONNECTION_OK) {
        helpers->finish_func(conn);
        return "Database Connection Error";
    }

    PGresult *res = PQexec(conn, "SELECT data FROM sample_table LIMIT 1;");
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        snprintf(result, sizeof(result), "%s", PQgetvalue(res, 0, 0));
    } else {
        snprintf(result, sizeof(result), "No Data Found");
    }

    PQclear(res);
    helpers->finish_func(conn);
    return result;
}

const char *fetch_data_with_stubs(
    PGconn *(*connectdb_func)(const char *),
    ConnStatusType (*status_func)(const PGconn *),
    PGresult *(*exec_func)(PGconn *, const char *),
    ExecStatusType (*result_status_func)(const PGresult *),
    int (*ntuples_func)(const PGresult *),
    char *(*getvalue_func)(const PGresult *, int, int),
    void (*clear_func)(PGresult *),
    void (*finish_func)(PGconn *)) {
    static char result[256];

    PGconn *conn = connectdb_func("host=localhost port=5432 dbname=testdb user=test password=test");
    if (status_func(conn) != CONNECTION_OK) {
        finish_func(conn);
        return "Database Error";
    }

    PGresult *res = exec_func(conn, "SELECT data FROM sample_table LIMIT 1;");
    if (result_status_func(res) == PGRES_TUPLES_OK && ntuples_func(res) > 0) {
        snprintf(result, sizeof(result), "%s", getvalue_func(res, 0, 0));
    } else {
        snprintf(result, sizeof(result), "No Data Found");
    }

    clear_func(res);
    finish_func(conn);
    return result;
}
