cmake_minimum_required(VERSION 3.16)
project(c_rest_postgres_api C)

set(CMAKE_C_STANDARD 11)

# Add all source files for the main executable
set(SOURCES
        src/main.c
        src/server.c
        src/data_handler.c
        src/data_service.c
        src/data_repository.c
        src/dotenv.c
        src/dotenv.h
        src/migration_manager.c
)

# Add test sources for unit testing
set(TEST_SOURCES
        test/test_data_repository.c
        src/data_repository.c
        test/test_dotenv.c
        src/dotenv.c
        test/test_main.c
        test/test_migration_manager.c
        src/migration_manager.c
)

# Include directories for headers
include_directories(
        src
        /usr/local/opt/libpq/include    # Path for libpq headers
        /usr/local/include              # Path for GnuTLS headers
        /usr/local/include/cgreen       # Path for Cgreen headers
        /usr/include/postgresql         # Path for ubuntu
)

# Link directories for libraries
link_directories(
        /usr/local/opt/libpq/lib        # Path for libpq library
        /usr/local/lib                  # Path for GnuTLS and Cgreen libraries
        /usr/lib/x86_64-linux-gnu       # Common Ubuntu path for libraries
)

# Create the main executable
add_executable(c_rest_postgres_api ${SOURCES})

# Link the required libraries to the main executable
target_link_libraries(c_rest_postgres_api pq gnutls)

# Create the test executable
add_executable(run_tests ${TEST_SOURCES})

# Link Cgreen and required libraries to the test executable
target_link_libraries(run_tests cgreen pq gnutls)

# Add Cgreen runner for unit tests
enable_testing()
add_test(NAME tests COMMAND cgreen-runner run_tests)
