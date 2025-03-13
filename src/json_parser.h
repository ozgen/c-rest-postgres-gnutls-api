//
// Created by moezgen on 1/17/25.
//

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "camera_repository.h"

// Parses JSON string into CameraMetadata structure
int parse_json_to_metadata(const char *json, CameraMetadata *metadata);

#endif // JSON_PARSER_H
