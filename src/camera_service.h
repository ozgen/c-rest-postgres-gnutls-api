//
// Created by moezgen on 1/17/25.
//

#ifndef CAMERA_SERVICE_H
#define CAMERA_SERVICE_H
#include "camera_metadata.h"

typedef struct {
    int status_code;
    char message[1024];
}CameraMetadataResponse;

CameraMetadataResponse handle_camera_onboarding(const CameraMetadata *metadata);
#endif //CAMERA_SERVICE_H
