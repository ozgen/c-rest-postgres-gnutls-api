// src/services/data_service.c
#include "data_service.h"
#include "data_repository.h"




const char *get_data_service() {
    DBConnHelpers conn_helpers = {
        .connect_func = PQconnectdb,
        .status_func = PQstatus,
        .finish_func = PQfinish,
        .error_func = PQerrorMessage // Assuming fetch_data uses this member
    };

    return fetch_data(&conn_helpers); // Pass the address of conn_helpers
}
