#include <stdlib.h>
#include <postgresql/libpq-fe.h>
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
    // Call the actual `apply_migration` function
    apply_migration(conn, filename, db_helpers);
}

int main() {
    // Load environment variables from the .env file
    if (dotenv_load("../dev.env") != 0) {
        fprintf(stderr, "Failed to load dev.env file\n");
        return 1;
    }

    // Read PostgreSQL connection details from environment variables
    const char *host = getenv("POSTGRES_HOST");
    const char *port = getenv("POSTGRES_PORT");
    const char *dbname = getenv("POSTGRES_DB");
    const char *user = getenv("POSTGRES_USER");
    const char *password = getenv("POSTGRES_PASSWORD");

    if (!host || !port || !dbname || !user || !password) {
        fprintf(stderr, "Database environment variables are not set properly.\n");
        return 1;
    }

    // Construct the connection string
    char conninfo[512];
    snprintf(conninfo, sizeof(conninfo),
             "host=%s port=%s dbname=%s user=%s password=%s",
             host, port, dbname, user, password);

    // Connect to the PostgreSQL database
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Database connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // Initialize DBHelpers struct
    DBHelpers db_helpers = {
        .exec_func = PQexec,
        .clear_func = clear_func,
        .finish_func= finish_func,
        .get_pqResultStatus = get_PQresultStatus
    };

    // Run database migrations
    printf("Running database migrations...\n");
    run_migrations(conn, "../migrations", &db_helpers, apply_migration_wrapper);

    // Start the server
    printf("Starting the server...\n");
    start_server();

    return 0;
}