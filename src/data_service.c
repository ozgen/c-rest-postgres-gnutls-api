// src/services/data_service.c
#include "data_service.h"
#include "data_repository.h"

const char *get_data_service() {
    return fetch_data();
}