#include <stdlib.h>

#include "db_connection.h"
#include "dotenv.h"
#include "server.h"
#include "migration_manager.h"

int main() {
    if (dotenv_load("../dev.env") != 0) {
        fprintf(stderr, "Failed to load dev.env file\n");
        return 1;
    }

    const DBHelpers helpers = {
        .connect_func = PQconnectdb,
        .status_func = PQstatus,
        .finish_func = PQfinish,
        .error_func = (const char *(*)(const PGconn *))PQerrorMessage,
        .exec_func = PQexec,
        .clear_func = PQclear,
        .get_pqResultStatus = PQresultStatus
    };

    char *conninfo = build_connection_string();
    if (!conninfo) {
        return 1;
    }

    PGconn *conn = create_connection(conninfo, &helpers);
    free(conninfo);

    if (!conn) {
        return 1;
    }

    printf("Running database migrations...\n");
    run_migrations(conn, "../migrations", &helpers, apply_migration);

    printf("Starting the server...\n");
    start_server();

    close_connection(conn, &helpers);
    return 0;
}
