cmake_minimum_required(VERSION 3.16)
project(c_rest_postgres_api C)

set(CMAKE_C_STANDARD 11)

# Include directories for headers
include_directories(
        src
        /usr/local/opt/libpq/include    # Path for libpq headers
        /usr/local/include              # Path for GnuTLS headers
        /usr/local/include/cgreen       # Path for Cgreen headers
        /usr/include/postgresql         # Path for Ubuntu
)

# Link directories for libraries
link_directories(
        /usr/local/opt/libpq/lib        # Path for libpq library
        /usr/local/lib                  # Path for GnuTLS and Cgreen libraries
        /usr/lib/x86_64-linux-gnu       # Common Ubuntu path for libraries
)

# Main application sources
set(MAIN_SOURCES
        src/main.c
        src/server.c
        src/data_handler.c
        src/data_service.c
        src/data_repository.c
        src/dotenv.c
        src/migration_manager.c
        src/db_connection.c
        src/camera_repository.c
        src/camera_service.c
        src/camera_handler.c
        src/json_parser.c
)

# Main application sources (excluding main.c for tests)
set(SOURCES
        src/server.c
        src/data_handler.c
        src/data_service.c
        src/data_repository.c
        src/dotenv.c
        src/migration_manager.c
        src/db_connection.c
        src/camera_repository.c
        src/camera_service.c
        src/camera_handler.c
        src/json_parser.c
)

# Main application executable
add_executable(c_rest_postgres_api ${MAIN_SOURCES})
target_link_libraries(c_rest_postgres_api pq gnutls)

# Define test files
set(TEST_FILES
        test/test_data_repository.c
        test/test_dotenv.c
        test/test_migration_manager.c
        test/test_db_connection.c
        test/test_json_parser.c
)

# Add Cgreen runner for unit tests
enable_testing()

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE) # Extract test name
    add_executable(${TEST_NAME} ${TEST_FILE} ${SOURCES})   # Create an executable for each test
    target_link_libraries(${TEST_NAME} PRIVATE cgreen pq gnutls) # Link libraries
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})       # Register the test
endforeach()

add_executable(test_camera_handler
        test/test_camera_handler.c
        src/camera_handler.c
        src/json_parser.c
)

# Define the mock function for `handle_camera_onboarding`
target_compile_definitions(test_camera_handler PRIVATE handle_camera_onboarding=handle_camera_onboarding_mock)
target_link_libraries(test_camera_handler PRIVATE cgreen pq gnutls)
add_test(NAME test_camera_handler COMMAND test_camera_handler)

