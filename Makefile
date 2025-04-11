# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = .

# Library files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
TARGET = html_gen.exe

# Test files
TEST_SIMPLE = simple_test.exe
TEST_COMPLEX = complex_test.exe

# Main targets
all: $(BUILD_DIR) $(TARGET) tests

# Build the test executables
tests: $(TEST_SIMPLE) $(TEST_COMPLEX)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@if not exist "$(BUILD_DIR)" mkdir $(BUILD_DIR)

# Link object files to create main executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files in build folder
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile and link simple test
$(TEST_SIMPLE): simple.c $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $< $(filter-out $(BUILD_DIR)/html_gen.o, $(OBJECTS))

# Compile and link complex test
$(TEST_COMPLEX): complex_test.c $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $< $(filter-out $(BUILD_DIR)/html_gen.o, $(OBJECTS))

# Run simple test
run_simple: $(TEST_SIMPLE)
	./$(TEST_SIMPLE)

# Run complex test
run_complex: $(TEST_COMPLEX)
	./$(TEST_COMPLEX)

# Clean up build artifacts
clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q $(BUILD_DIR)
	@if exist "$(TARGET)" del /Q $(TARGET)
	@if exist "$(TEST_SIMPLE)" del /Q $(TEST_SIMPLE)
	@if exist "$(TEST_COMPLEX)" del /Q $(TEST_COMPLEX)
	@if exist "simple_output.html" del /Q simple_output.html
	@if exist "complex_output.html" del /Q complex_output.html

.PHONY: all tests clean run_simple run_complex
