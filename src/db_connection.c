//
// Created by moezgen on 1/16/25.
//

#include "db_connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to create a new database connection
PGconn *create_connection(const char *conninfo, const DBHelpers * conn_helper) {
    PGconn *conn = conn_helper->connect_func(conninfo);
    if (conn_helper->status_func(conn) != CONNECTION_OK) {
        fprintf(stderr, "Database connection failed: %s\n", conn_helper->error_func(conn));
        conn_helper->finish_func(conn);
        return NULL;
    }
    return conn;
}

// Function to close an existing database connection
void close_connection(PGconn *conn, const DBHelpers * conn_helper) {
    if (conn) {
        conn_helper->finish_func(conn);
    }
}

// Function to build a connection string from environment variables
char *build_connection_string() {
    const char *host = getenv("POSTGRES_HOST");
    const char *port = getenv("POSTGRES_PORT");
    const char *dbname = getenv("POSTGRES_DB");
    const char *user = getenv("POSTGRES_USER");
    const char *password = getenv("POSTGRES_PASSWORD");

    if (!host || !port || !dbname || !user || !password) {
        fprintf(stderr, "Database environment variables are not set properly.\n");
        return NULL;
    }

    char *conninfo = (char *)malloc(512);
    snprintf(conninfo, 512, "host=%s port=%s dbname=%s user=%s password=%s",
             host, port, dbname, user, password);
    return conninfo;
}

