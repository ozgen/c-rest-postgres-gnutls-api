//
// Created by moezgen on 1/17/25.
//

#ifndef CAMERA_METADATA_H
#define CAMERA_METADATA_H

#include <time.h>

// Define CameraMetadata struct
typedef struct {
    char cam_id[37];
    char image_id[37];
    char camera_name[256];
    char firmware_version[256];
    char container_name[256];
    char name_of_stored_picture[256];
    time_t created_at;
    time_t onboarded_at;
    time_t initialized_at;
} CameraMetadata;

#endif // CAMERA_METADATA_H
