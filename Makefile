CC = gcc
CFLAGS = -I/usr/local/include -I/usr/local/opt/libpq/include -I/usr/include/postgresql -I/usr/local/include/cgreen -Isrc
LDFLAGS = -L/usr/local/lib -L/usr/local/opt/libpq/lib -L/usr/lib/x86_64-linux-gnu
LIBS = -lpq -lgnutls -lcgreen
BUILD_DIR = build
SRCS = src/main.c src/server.c src/data_handler.c src/data_service.c src/data_repository.c src/dotenv.c
TEST_SRCS = test/test_data_repository.c src/data_repository.c test/test_dotenv.c src/dotenv.c test/test_main.c src/migration_manager.c test/test_migration_manager.c
OBJS = $(SRCS:src/%.c=$(BUILD_DIR)/%.o)
TEST_OBJS = $(TEST_SRCS:test/%.c=$(BUILD_DIR)/%.o)

# The default target
all: $(BUILD_DIR) main run_tests

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the main executable
main: $(OBJS)
	$(CC) $(OBJS) -o $(BUILD_DIR)/main $(LDFLAGS) $(LIBS)

# Build the test executable
run_tests: $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(BUILD_DIR)/run_tests $(LDFLAGS) $(LIBS)

# Rule for compiling each source file
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling each test source file
$(BUILD_DIR)/%.o: test/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)

# Run tests
test: run_tests
	$(BUILD_DIR)/run_tests
