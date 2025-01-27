//
// Created by moezgen on 1/27/25.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "json_parser.h"


Describe(JsonParser);
BeforeEach(JsonParser) {}
AfterEach(JsonParser) {}

// Test case: Successfully parses metadata
Ensure(JsonParser, parse_json_to_metadata_successfully_parses_valid_json) {
    const char *json = "{\"camera_name\":\"Camera1\",\"firmware_version\":\"v1.2.3\"}";
    CameraMetadata metadata;

    int result = parse_json_to_metadata(json, &metadata);

    assert_that(result, is_equal_to(1));
    assert_that(strcmp(metadata.camera_name, "Camera1"), is_equal_to(0));
    assert_that(strcmp(metadata.firmware_version, "v1.2.3"), is_equal_to(0));
}

// Test case: Fails for invalid JSON
Ensure(JsonParser, parse_json_to_metadata_fails_for_invalid_json) {
    const char *json = "{\"invalid_key\":\"value\"}";
    CameraMetadata metadata;

    int result = parse_json_to_metadata(json, &metadata);

    assert_that(result, is_equal_to(-1));
}

// Test case: Fails for missing keys
Ensure(JsonParser, parse_json_to_metadata_fails_for_missing_keys) {
    const char *json = "{\"camera_name\":\"Camera1\"}"; // Missing firmware_version
    CameraMetadata metadata;

    int result = parse_json_to_metadata(json, &metadata);

    assert_that(result, is_equal_to(-1));
}

// Test case: Handles extra unexpected keys
Ensure(JsonParser, parse_json_to_metadata_ignores_extra_keys) {
    const char *json = "{\"camera_name\":\"Camera1\",\"firmware_version\":\"v1.2.3\",\"extra_key\":\"extra_value\"}";
    CameraMetadata metadata;

    int result = parse_json_to_metadata(json, &metadata);

    assert_that(result, is_equal_to(1));
    assert_that(strcmp(metadata.camera_name, "Camera1"), is_equal_to(0));
    assert_that(strcmp(metadata.firmware_version, "v1.2.3"), is_equal_to(0));
}

// Test case: Fails when JSON is empty
Ensure(JsonParser, parse_json_to_metadata_fails_for_empty_json) {
    const char *json = "{}"; // Empty JSON
    CameraMetadata metadata;

    int result = parse_json_to_metadata(json, &metadata);

    assert_that(result, is_equal_to(-1));
}

// Test case: Fails for null JSON input
Ensure(JsonParser, parse_json_to_metadata_fails_for_null_input) {
    CameraMetadata metadata;

    int result = parse_json_to_metadata(NULL, &metadata);

    assert_that(result, is_equal_to(-1));
}

// Group all `parse_json_to_metadata` tests into a test suite
TestSuite *JsonParser_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, JsonParser, parse_json_to_metadata_successfully_parses_valid_json);
    add_test_with_context(suite, JsonParser, parse_json_to_metadata_fails_for_invalid_json);
    add_test_with_context(suite, JsonParser, parse_json_to_metadata_fails_for_missing_keys);
    add_test_with_context(suite, JsonParser, parse_json_to_metadata_ignores_extra_keys);
    add_test_with_context(suite, JsonParser, parse_json_to_metadata_fails_for_empty_json);
    add_test_with_context(suite, JsonParser, parse_json_to_metadata_fails_for_null_input);
    return suite;
}

int main(int argc, char **argv) {
    TestSuite *suite = create_test_suite();
    add_suite(suite, JsonParser_tests());
    return run_test_suite(suite, create_text_reporter());
}
