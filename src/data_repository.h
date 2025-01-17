#ifndef DATA_REPOSITORY_H
#define DATA_REPOSITORY_H
#include <postgresql/libpq-fe.h>
#include "db_connection.h"


const char *fetch_data(DBConnHelpers *);
const char *fetch_data_with_stubs(
    PGconn *(*connectdb_func)(const char *),
    ConnStatusType (*status_func)(const PGconn *),
    PGresult *(*exec_func)(PGconn *, const char *),
    ExecStatusType (*result_status_func)(const PGresult *),
    int (*ntuples_func)(const PGresult *),
    char *(*getvalue_func)(const PGresult *, int, int),
    void (*clear_func)(PGresult *),
    void (*finish_func)(PGconn *));

#endif