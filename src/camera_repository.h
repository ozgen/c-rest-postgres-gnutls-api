//
// Created by moezgen on 1/17/25.
//

#ifndef CAMERA_REPOSITORY_H
#define CAMERA_REPOSITORY_H

#include "camera_metadata.h"
#include "db_connection.h"

typedef enum {
    CAMERA_SUCCESS = 0,
    CAMERA_ERROR_DB_CONFIG = 1,
    CAMERA_ERROR_DB_CONNECTION = 2,
    CAMERA_ERROR_INSERT_FAILED = 3,
    CAMERA_ERROR_ALREADY_EXISTS = 4
} CameraError;


CameraError save_camera_metadata(const CameraMetadata *metadata, const DBHelpers *db_helper);

#endif //CAMERA_REPOSITORY_H
