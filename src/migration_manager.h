//
// Created by moezgen on 1/13/25.
//
#include <postgresql/libpq-fe.h>


#ifndef MIGRATION_MANAGER_H
#define MIGRATION_MANAGER_H

// Function pointer types for dependency injection
typedef PGconn *(*ConnectFunc)(const void *);
typedef PGresult *(*ExecFunc)(PGconn *, const char *);
typedef ExecStatusType (*ResultStatusFunc)(const PGresult *);
typedef void (*ClearFunc)(PGresult *);
typedef void (*FinishFunc)(PGconn *);

typedef struct {
    PGresult *(*exec_func)(PGconn *, const char *);
    void (*clear_func)(PGresult *);
    void (*finish_func)(PGconn *);
    ExecStatusType (*get_pqResultStatus)(const PGresult *res);
} DBHelpers;


// Standard migrations runner
void run_migrations(
    PGconn *conn,
    const char *migrations_path,
    DBHelpers *db_helpers,
    void (*apply_migration_func)(PGconn *, const char *, DBHelpers *));

// Migrations runner with dependency injection for testing
void apply_migration(PGconn *conn, const char *filename, DBHelpers *db_helpers);

#endif // MIGRATION_MANAGER_H
