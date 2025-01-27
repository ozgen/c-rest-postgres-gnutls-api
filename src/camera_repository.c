//
// Created by moezgen on 1/17/25.
//

#include "camera_repository.h"
#include <stdlib.h>


#include <stdbool.h> // For boolean handling in optional fields
#include <string.h>

CameraError save_camera_metadata(const CameraMetadata *metadata, const DBHelpers *db_helper) {
    char *conninfo = build_connection_string();
    if (!conninfo) {
        fprintf(stderr, "Database configuration error.\n");
        return CAMERA_ERROR_DB_CONFIG;
    }

    PGconn *conn = create_connection(conninfo, db_helper);
    free(conninfo);

    if (!conn) {
        fprintf(stderr, "Database connection failed.\n");
        return CAMERA_ERROR_DB_CONNECTION;
    }

    // Check if record already exists
    char check_query[256];
    snprintf(check_query, sizeof(check_query),
             "SELECT cam_id FROM camera_metadata WHERE cam_id = '%s'", metadata->cam_id);

    PGresult *check_res = db_helper->exec_func(conn, check_query);
    if (db_helper->get_pqResultStatus(check_res) == PGRES_TUPLES_OK && PQntuples(check_res) > 0) {
        fprintf(stderr, "Record already exists for cam_id: %s\n", metadata->cam_id);
        db_helper->clear_func(check_res);
        close_connection(conn, db_helper);
        return CAMERA_ERROR_ALREADY_EXISTS;
    }
    db_helper->clear_func(check_res);

    // Construct SQL query dynamically
    char query[2048];
    snprintf(query, sizeof(query),
            "INSERT INTO camera_metadata ("
            "image_id, camera_name, firmware_version, container_name, "
            "name_of_stored_picture, created_at, onboarded_at, initialized_at) "
            "VALUES (%s, '%s', '%s', %s, %s, NOW(), %s, %s)",
            metadata->image_id && strlen(metadata->image_id) > 0 ? metadata->image_id : "NULL",
            metadata->camera_name,
            metadata->firmware_version,
            metadata->container_name && strlen(metadata->container_name) > 0 ? metadata->container_name : "NULL",
            metadata->name_of_stored_picture && strlen(metadata->name_of_stored_picture) > 0 ? metadata->name_of_stored_picture : "NULL",
            metadata->onboarded_at ? metadata->onboarded_at : "NULL",
            metadata->initialized_at ? metadata->initialized_at : "NULL");



    PGresult *res = db_helper->exec_func(conn, query);

    if (db_helper->get_pqResultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Insert failed: %s\n", db_helper->error_func(conn));
        db_helper->clear_func(res);
        close_connection(conn, db_helper);
        return CAMERA_ERROR_INSERT_FAILED;
    }

    db_helper->clear_func(res);
    close_connection(conn, db_helper);
    return CAMERA_SUCCESS;
}
