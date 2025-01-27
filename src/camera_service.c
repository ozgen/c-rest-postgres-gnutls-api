//
// Created by moezgen on 1/17/25.
//

#include "camera_service.h"
#include "camera_repository.h"
const DBHelpers db_helpers = {
    .connect_func = PQconnectdb,
    .status_func = PQstatus,
    .finish_func = PQfinish,
    .error_func = (const char *(*)(const PGconn *)) PQerrorMessage,
    .exec_func = PQexec,
    .clear_func = PQclear,
    .get_pqResultStatus = PQresultStatus
};

CameraMetadataResponse handle_camera_onboarding(const CameraMetadata *metadata) {
    CameraError result = save_camera_metadata(metadata, &db_helpers);

    CameraMetadataResponse response;

    switch (result) {
        case CAMERA_SUCCESS:
            response.status_code = 201; // Created
            snprintf(response.message, sizeof(response.message),
                     "Camera metadata saved successfully for camera_name: %s.", metadata->camera_name);
            break;
        case CAMERA_ERROR_ALREADY_EXISTS:
            response.status_code = 409; // Conflict
            snprintf(response.message, sizeof(response.message), "Camera metadata already exists for camera_name: %s.",
                     metadata->camera_name);
            break;
        case CAMERA_ERROR_DB_CONFIG:
            response.status_code = 500; // Internal Server Error
            snprintf(response.message, sizeof(response.message), "Database configuration error.");
            break;
        case CAMERA_ERROR_DB_CONNECTION:
            response.status_code = 500; // Internal Server Error
            snprintf(response.message, sizeof(response.message), "Database connection failed.");
            break;
        case CAMERA_ERROR_INSERT_FAILED:
            response.status_code = 500; // Internal Server Error
            snprintf(response.message, sizeof(response.message),
                     "Failed to insert camera metadata for camera_name: %s.", metadata->camera_name);
            break;
        default:
            response.status_code = 500; // Internal Server Error
            snprintf(response.message, sizeof(response.message),
                     "Unknown error occurred while saving camera metadata.");
            break;
    }

    return response;
}
