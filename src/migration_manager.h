//
// Created by moezgen on 1/13/25.
//
#include <postgresql/libpq-fe.h>
#include "db_connection.h"

#ifndef MIGRATION_MANAGER_H
#define MIGRATION_MANAGER_H

void apply_migration(PGconn *conn, const char *filename, const DBHelpers *db_helpers);

// Standard migrations runner
void run_migrations(
    PGconn *conn,
    const char *migrations_path,
    const DBHelpers *db_helpers,
    void (*apply_migration_func)(PGconn *, const char *, const DBHelpers *)
);

#endif // MIGRATION_MANAGER_H
