#include <stdlib.h>

#include "db_connection.h"
#include "dotenv.h"
#include "server.h"
#include "migration_manager.h"

// Clear function for cleaning PGresult
void clear_func(PGresult *res) {
    PQclear(res);
}

// Finish function for cleaning up PGconn
void finish_func(PGconn *conn) {
    PQfinish(conn);
}

ExecStatusType get_PQresultStatus(PGresult *res) {
    return PQresultStatus(res);
}

void apply_migration_wrapper(PGconn *conn, const char *filename, DBHelpers *db_helpers) {
    apply_migration(conn, filename, db_helpers);
}

int main() {
    if (dotenv_load("../dev.env") != 0) {
        fprintf(stderr, "Failed to load dev.env file\n");
        return 1;
    }

    DBConnHelpers conn_helpers = {
        .connect_func = PQconnectdb,
        .status_func = PQstatus,
        .finish_func = PQfinish,
        .error_func = PQerrorMessage
    };

    char *conninfo = build_connection_string();
    if (!conninfo) {
        return 1;
    }

    PGconn *conn = create_connection(conninfo, &conn_helpers);
    free(conninfo);

    if (!conn) {
        return 1;
    }

    DBHelpers db_helpers = {
        .exec_func = PQexec,
        .clear_func = clear_func,
        .finish_func = finish_func,
        .get_pqResultStatus = get_PQresultStatus
    };

    printf("Running database migrations...\n");
    run_migrations(conn, "../migrations", &db_helpers, apply_migration_wrapper);

    printf("Starting the server...\n");
    start_server();

    close_connection(conn, &conn_helpers);
    return 0;
}
