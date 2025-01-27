//
// Created by moezgen on 1/16/25.
//

#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <postgresql/libpq-fe.h>

typedef struct {
    PGconn *(*connect_func)(const char *); // Match PQconnectdb
    ConnStatusType (*status_func)(const PGconn *); // Match PQstatus
    void (*finish_func)(PGconn *); // Match PQfinish
    const char *(*error_func)(const PGconn *); // Match PQerrorMessage
    PGresult *(*exec_func)(PGconn *, const char *);  // Match PQexec
    void (*clear_func)(PGresult *);  // Match PQclear
    ExecStatusType (*get_pqResultStatus)(const PGresult *res);  // Match PQresultStatus
} DBHelpers;


// Function to create a new database connection
PGconn *create_connection(const char *conninfo, const DBHelpers *helpers);

// Function to close an existing database connection
void close_connection(PGconn *conn, const DBHelpers *helpers);

// Function to build a connection string from environment variables
char *build_connection_string();

#endif // DB_CONNECTION_H
