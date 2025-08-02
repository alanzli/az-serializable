# Makefile for serialization framework
CXX = g++
HEADER_DIR = headers
INCLUDES = -I$(HEADER_DIR)
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(INCLUDES)
BUILD_DIR = out
TEST_DIR = tests
EXAMPLE_TARGET = $(BUILD_DIR)/example
CONTAINER_TARGET = $(BUILD_DIR)/container_example
SIMPLE_TYPES_TARGET = $(BUILD_DIR)/simple_types_example
SIMPLE_TEST_TARGET = $(BUILD_DIR)/simple_test
EXAMPLE_SOURCES = $(TEST_DIR)/UsageExample.cpp
CONTAINER_SOURCES = $(TEST_DIR)/ContainerExample.cpp
SIMPLE_TYPES_SOURCES = $(TEST_DIR)/SimpleTypesExample.cpp
SIMPLE_TEST_SOURCES = $(TEST_DIR)/SimpleTypesTest.cpp

# Build directory target
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Example target
$(EXAMPLE_TARGET): $(BUILD_DIR) $(EXAMPLE_SOURCES) $(HEADER_DIR)/*.h
	$(CXX) $(CXXFLAGS) -o $(EXAMPLE_TARGET) $(EXAMPLE_SOURCES)

# Container example target
$(CONTAINER_TARGET): $(BUILD_DIR) $(CONTAINER_SOURCES) $(HEADER_DIR)/*.h
	$(CXX) $(CXXFLAGS) -o $(CONTAINER_TARGET) $(CONTAINER_SOURCES)

# Simple types example target
$(SIMPLE_TYPES_TARGET): $(BUILD_DIR) $(SIMPLE_TYPES_SOURCES) $(HEADER_DIR)/*.h
	$(CXX) $(CXXFLAGS) -o $(SIMPLE_TYPES_TARGET) $(SIMPLE_TYPES_SOURCES)

# Simple test target
$(SIMPLE_TEST_TARGET): $(BUILD_DIR) $(SIMPLE_TEST_SOURCES) $(HEADER_DIR)/*.h
	$(CXX) $(CXXFLAGS) -o $(SIMPLE_TEST_TARGET) $(SIMPLE_TEST_SOURCES)

# Build all
all: $(EXAMPLE_TARGET) $(CONTAINER_TARGET) $(SIMPLE_TYPES_TARGET) $(SIMPLE_TEST_TARGET)

# Run example
run-example: $(EXAMPLE_TARGET)
	./$(EXAMPLE_TARGET)

# Run container example
run-containers: $(CONTAINER_TARGET)
	./$(CONTAINER_TARGET)

# Run simple types example
run-simple-types: $(SIMPLE_TYPES_TARGET)
	./$(SIMPLE_TYPES_TARGET)

# Clean
# Run simple test
run-simple-test: $(SIMPLE_TEST_TARGET)
	./$(SIMPLE_TEST_TARGET)

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)

# Test all implementations
test: all
	@echo "=== Running Example Implementation ==="
	./$(EXAMPLE_TARGET)
	@echo ""
	@echo "=== Running Container Implementation ==="
	./$(CONTAINER_TARGET)
	@echo ""
	@echo "=== Running Simple Types Implementation ==="
	./$(SIMPLE_TYPES_TARGET)
	@echo ""
	@echo "=== Running Simple Test ==="
	./$(SIMPLE_TEST_TARGET)

.PHONY: all run-example run-containers run-simple-types run-simple-test clean test
