//
// Created by moezgen on 1/27/25.
//

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <string.h>
#include "camera_handler.h"
#include "camera_service.h"

// Use macro to ensure the mock function overrides the real implementation
#define handle_camera_onboarding handle_camera_onboarding_mock

Describe(CameraService);

BeforeEach(CameraService) {
}

AfterEach(CameraService) {
}

// Mock for handle_camera_onboarding
CameraMetadataResponse handle_camera_onboarding_mock(const CameraMetadata *metadata) {
    printf("DEBUG: Mock handle_camera_onboarding called\n");
    printf("DEBUG: Mock received metadata: camera_name=%s, firmware_version=%s\n",
           metadata->camera_name, metadata->firmware_version);
    if (metadata->camera_name == NULL || metadata->firmware_version == NULL) {
        return (CameraMetadataResponse){400, "Invalid metadata: missing required fields"};
    }
    return *(CameraMetadataResponse *) mock(metadata);
}

// Test case: Valid JSON and successful onboarding
Ensure(CameraService, valid_json_creates_camera_successfully) {
    const char *body = "{\"camera_name\":\"Camera1\",\"firmware_version\":\"v1.0.0\"}";
    char response[1024] = {0};

    // Define the expected metadata and response
    CameraMetadata expected_metadata = {.camera_name = "Camera1", .firmware_version = "v1.0.0"};
    static CameraMetadataResponse expected_response = {201, "Camera successfully onboarded"};

    // Configure the mock to return the expected response
    expect(handle_camera_onboarding_mock,
           when(metadata, is_equal_to_contents_of(&expected_metadata, sizeof(CameraMetadata))),
           will_return(&expected_response));

    // Call the function under test
    handle_camera_post_request("/cameras", body, response);

    // Validate the response
    assert_that(strstr(response, "201 Created"), is_non_null);
    assert_that(strstr(response, "Camera successfully onboarded"), is_non_null);
}

Ensure(CameraService, null_metadata_returns_400) {
    const char *body = "{\"camera_name\":,\"firmware_version\":}";
    char response[1024] = {0};

    // Mock to simulate the null metadata scenario
    CameraMetadataResponse expected_response = {400, "Invalid metadata: missing required fields"};
    expect(handle_camera_onboarding_mock,
           will_return(&expected_response));

    // Call the function under test
    handle_camera_post_request("/cameras", body, response);

    // Validate the response
    assert_that(strstr(response, "400 Bad Request"), is_non_null);
    assert_that(strstr(response, "Invalid metadata: missing required fields"), is_non_null);
}

// Test case: Conflict response
Ensure(CameraService, conflict_response_returns_409) {
    const char *body = "{\"camera_name\":\"Camera2\",\"firmware_version\":\"v2.0.0\"}";
    char response[1024] = {0};

    // Define the expected metadata and response
    CameraMetadata expected_metadata = {.camera_name = "Camera2", .firmware_version = "v2.0.0"};
    static CameraMetadataResponse expected_response = {409, "Camera already exists"};

    // Configure the mock to return the expected response
    expect(handle_camera_onboarding_mock,
           when(metadata, is_equal_to_contents_of(&expected_metadata, sizeof(CameraMetadata))),
           will_return(&expected_response));

    // Call the function under test
    handle_camera_post_request("/cameras", body, response);

    // Validate the response
    assert_that(strstr(response, "409 Conflict"), is_non_null);
    assert_that(strstr(response, "Camera already exists"), is_non_null);
}

// Test case: Internal server error
Ensure(CameraService, internal_server_error_returns_500) {
    const char *body = "{\"camera_name\":\"Camera3\",\"firmware_version\":\"v3.0.0\"}";
    char response[1024] = {0};

    // Define the expected metadata and response
    CameraMetadata expected_metadata = {.camera_name = "Camera3", .firmware_version = "v3.0.0"};
    static CameraMetadataResponse expected_response = {500, "Database configuration error"};

    // Configure the mock to return the expected response
    expect(handle_camera_onboarding_mock,
           when(metadata, is_equal_to_contents_of(&expected_metadata, sizeof(CameraMetadata))),
           will_return(&expected_response));

    // Call the function under test
    handle_camera_post_request("/cameras", body, response);

    // Validate the response
    assert_that(strstr(response, "500 Internal Server Error"), is_non_null);
    assert_that(strstr(response, "Database configuration error"), is_non_null);
}

// Group all tests into a test suite
TestSuite *CameraHandler_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, CameraService, valid_json_creates_camera_successfully);
    add_test_with_context(suite, CameraService, null_metadata_returns_400);
    add_test_with_context(suite, CameraService, conflict_response_returns_409);
    add_test_with_context(suite, CameraService, internal_server_error_returns_500);
    return suite;
}

int main(int argc, char **argv) {
    TestSuite *suite = create_test_suite();
    add_suite(suite, CameraHandler_tests());
    return run_test_suite(suite, create_text_reporter());
}
