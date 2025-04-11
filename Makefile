# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = .

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
TARGET = html_gen.exe

# Main target
all: $(BUILD_DIR) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@if not exist "$(BUILD_DIR)" mkdir $(BUILD_DIR)

# Link object files to create executable in root folder
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files in build folder
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up build artifacts
clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q $(BUILD_DIR)
	@if exist "$(TARGET)" del /Q $(TARGET)

.PHONY: all clean
