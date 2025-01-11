#ifndef DATA_REPOSITORY_H
#define DATA_REPOSITORY_H
#include <libpq-fe.h>

const char *fetch_data();
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