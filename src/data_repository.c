#include "data_repository.h"
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>

const char *fetch_data() {
    static char result[256];

    // Read PostgreSQL connection details from environment variables
    const char *host = getenv("POSTGRES_HOST");
    const char *port = getenv("POSTGRES_PORT");
    const char *dbname = getenv("POSTGRES_DB");
    const char *user = getenv("POSTGRES_USER");
    const char *password = getenv("POSTGRES_PASSWORD");

    if (!host || !port || !dbname || !user || !password) {
        fprintf(stderr, "Database environment variables are not set properly.\n");
        return "Database Configuration Error";
    }

    // Construct the connection string
    char conninfo[512];
    snprintf(conninfo, sizeof(conninfo),
             "host=%s port=%s dbname=%s user=%s password=%s",
             host, port, dbname, user, password);

    // Connect to the PostgreSQL database
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return "Database Connection Error";
    }

    PGresult *res = PQexec(conn, "SELECT data FROM sample_table LIMIT 1;");
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
        snprintf(result, sizeof(result), "%s", PQgetvalue(res, 0, 0));
    } else {
        snprintf(result, sizeof(result), "No Data Found");
    }

    PQclear(res);
    PQfinish(conn);
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
