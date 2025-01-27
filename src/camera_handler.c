//
// Created by moezgen on 1/17/25.
//

#include "camera_service.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "json_parser.h"



void safe_snprintf(char *response, size_t response_size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(response, response_size, format, args);
    va_end(args);
}

void handle_camera_post_request(const char *request, const char *body, char *response) {
    // Check if the request matches the POST endpoint
    CameraMetadata metadata = {0};

    // Parse JSON payload from `body` (assume `parse_json_to_metadata` exists)
    if (!parse_json_to_metadata(body, &metadata)) {
        snprintf(response, 1024,
                 "HTTP/1.1 400 Bad Request\r\n"
                 "Content-Length: 27\r\n"
                 "\r\n"
                 "{\"error\": \"Invalid JSON\"}");
        return;
    }

    printf("DEBUG: Parsed metadata: camera_name=%s, firmware_version=%s\n", metadata.camera_name, metadata.firmware_version);

    CameraMetadataResponse result = handle_camera_onboarding(&metadata);

    printf("DEBUG: Onboarding result: status_code=%d, message=%s\n", result.status_code, result.message);

    // Prepare response based on the service layer result
    switch (result.status_code) {
        case 201: // Created
            safe_snprintf(response, 1024,
                     "HTTP/1.1 201 Created\r\n"
                     "Content-Length: %lu\r\n"
                     "\r\n"
                     "{\"message\": \"%s\"}",
                     strlen(result.message) + 23, result.message);
            break;
        case 409: // Conflict
            safe_snprintf(response, 1024,
                     "HTTP/1.1 409 Conflict\r\n"
                     "Content-Length: %lu\r\n"
                     "\r\n"
                     "{\"error\": \"%s\"}",
                     strlen(result.message) + 23, result.message);
            break;
        case 400: // Bad Request
            safe_snprintf(response, 1024,
                     "HTTP/1.1 400 Bad Request\r\n"
                     "Content-Length: %lu\r\n"
                     "\r\n"
                     "{\"error\": \"%s\"}",
                     strlen(result.message) + 23, result.message);
            break;
        case 500: // Internal Server Error
        default:
            safe_snprintf(response, 1024,
                     "HTTP/1.1 500 Internal Server Error\r\n"
                     "Content-Length: %lu\r\n"
                     "\r\n"
                     "{\"error\": \"%s\"}",
                     strlen(result.message) + 23, result.message);
            break;
    }
}

