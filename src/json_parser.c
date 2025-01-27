//
// Created by moezgen on 1/17/25.
//

#include "json_parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Helper function to extract a value by key
static int extract_json_value(const char *json, const char *key, char *value, size_t value_size) {
    char key_pattern[128];
    snprintf(key_pattern, sizeof(key_pattern), "\"%s\":", key);

    const char *key_start = strstr(json, key_pattern);
    if (!key_start) {
        return -1; // Key not found
    }

    const char *value_start = strchr(key_start + strlen(key_pattern), '"');
    if (!value_start) {
        return -1; // Value not found
    }

    value_start++; // Skip the opening quote
    const char *value_end = strchr(value_start, '"');
    if (!value_end) {
        return -1; // Closing quote not found
    }

    size_t length = value_end - value_start;
    if (length >= value_size) {
        return -1; // Value too large for buffer
    }

    strncpy(value, value_start, length);
    value[length] = '\0';
    return 0;
}

int parse_json_to_metadata(const char *json, CameraMetadata *metadata) {
    if (json == NULL || metadata == NULL) {
        return -1; // Return error if input is NULL
    }
    const char *camera_name_key = "\"camera_name\":";
    const char *firmware_version_key = "\"firmware_version\":";

    const char *camera_name_start = strstr(json, camera_name_key);
    const char *firmware_version_start = strstr(json, firmware_version_key);

    if (!camera_name_start || !firmware_version_start) {
        return -1; // Invalid JSON keys
    }

    // Extract `camera_name`
    camera_name_start += strlen(camera_name_key);
    if (sscanf(camera_name_start, " \"%[^\"]\"", metadata->camera_name) != 1) {
        return -1; // Failed to parse camera_name
    }

    // Extract `firmware_version`
    firmware_version_start += strlen(firmware_version_key);
    if (sscanf(firmware_version_start, " \"%[^\"]\"", metadata->firmware_version) != 1) {
        return -1; // Failed to parse firmware_version
    }

    return 1; // Successfully parsed
}

