# Makefile for Simple C Logging Library
#
# repo   : https://github.com/ItsCbass/loggin.c
# author : https://github.com/ItsCbass
#
# Developed with ❤️ by Sebastian Rivera.

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = -lpthread

# Directories
LIB_DIR = lib
EXAMPLES_DIR = examples
BUILD_DIR = build

# Source files
LIBRARY_SOURCES = $(LIB_DIR)/logger/loggin.c
LIBRARY_OBJECTS = $(BUILD_DIR)/loggin.o
LIBRARY_ARCHIVE = $(BUILD_DIR)/libloggin.a

# Example sources
EXAMPLE_SOURCES = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_TARGETS = $(EXAMPLE_SOURCES:$(EXAMPLES_DIR)/%.c=$(BUILD_DIR)/%)

# Default target
all: $(LIBRARY_ARCHIVE) examples

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the static library
$(LIBRARY_ARCHIVE): $(LIBRARY_OBJECTS) | $(BUILD_DIR)
	ar rcs $@ $^

# Compile library object
$(BUILD_DIR)/loggin.o: $(LIB_DIR)/logger/loggin.c $(LIB_DIR)/loggin.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build examples
examples: $(EXAMPLE_TARGETS)

# Build individual examples
$(BUILD_DIR)/%: $(EXAMPLES_DIR)/%.c $(LIBRARY_ARCHIVE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -L$(BUILD_DIR) -lloggin $(LDFLAGS) -o $@

# Run examples
run-basic: $(BUILD_DIR)/basic_example
	./$(BUILD_DIR)/basic_example

run-file: $(BUILD_DIR)/file_output_example
	./$(BUILD_DIR)/file_output_example

run-advanced: $(BUILD_DIR)/advanced_example
	./$(BUILD_DIR)/advanced_example

# Run all examples
run-all: run-basic run-file run-advanced

# Run tests
test: $(BUILD_DIR)/logger.test
	./$(BUILD_DIR)/logger.test

# Build test executable
$(BUILD_DIR)/logger.test: $(LIB_DIR)/logger/logger.test.c $(LIBRARY_ARCHIVE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -L$(BUILD_DIR) -lloggin $(LDFLAGS) -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f *.log

# Install library (optional)
install: $(LIBRARY_ARCHIVE)
	@echo "Installing library to /usr/local/lib and headers to /usr/local/include"
	@echo "Note: This requires sudo privileges"
	sudo cp $(LIBRARY_ARCHIVE) /usr/local/lib/
	sudo cp $(LIB_DIR)/loggin.h /usr/local/include/

# Uninstall library (optional)
uninstall:
	@echo "Uninstalling library"
	@echo "Note: This requires sudo privileges"
	sudo rm -f /usr/local/lib/libloggin.a
	sudo rm -f /usr/local/include/loggin.h

# Show help
help:
	@echo "Available targets:"
	@echo "  all          - Build library and examples (default)"
	@echo "  examples     - Build all example programs"
	@echo "  run-basic    - Run basic example"
	@echo "  run-file     - Run file output example"
	@echo "  run-advanced - Run advanced example"
	@echo "  run-all      - Run all examples"
	@echo "  test         - Run test suite"
	@echo "  clean        - Remove build artifacts and log files"
	@echo "  install      - Install library system-wide (requires sudo)"
	@echo "  uninstall    - Uninstall library (requires sudo)"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all examples run-basic run-file run-advanced run-all test clean install uninstall help
