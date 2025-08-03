# Makefile for serialization framework
ifeq ($(shell uname), Darwin)
    CXX = clang++
else
    CXX = g++
endif
HEADER_DIR = az
INCLUDES = -I.
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(INCLUDES)
BUILD_DIR = out
DIST_DIR = dist
EXAMPLE_DIR = examples
EXAMPLE_TARGET = $(BUILD_DIR)/example
VALIDATOR_TARGET = $(BUILD_DIR)/validator_example
EXAMPLE_SOURCES = $(EXAMPLE_DIR)/UsageExample.cpp
VALIDATOR_SOURCES = $(EXAMPLE_DIR)/ValidatorExample.cpp

SOURCES_HEADER = $(HEADER_DIR)/*.h $(HEADER_DIR)/json/*.h

# Example target
$(EXAMPLE_TARGET): $(EXAMPLE_SOURCES) $(SOURCES_HEADER)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(EXAMPLE_TARGET) $(EXAMPLE_SOURCES)

# Validator example target
$(VALIDATOR_TARGET): $(VALIDATOR_SOURCES) $(SOURCES_HEADER)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(VALIDATOR_TARGET) $(VALIDATOR_SOURCES)

# Build all
all: $(EXAMPLE_TARGET) $(VALIDATOR_TARGET) 

# Run original example
run: $(EXAMPLE_TARGET)
	./$(EXAMPLE_TARGET)

# Run validator example
run-validator: $(VALIDATOR_TARGET)
	./$(VALIDATOR_TARGET)

# Clean build directory
clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DIST_DIR)

# Test implementation
test: all
	echo "Running basic serialization example..."
	./$(EXAMPLE_TARGET)
	echo "Running validator example..."
	./$(VALIDATOR_TARGET)

dist: 
	@mkdir -p $(DIST_DIR)
	@cp -rf $(HEADER_DIR) $(DIST_DIR)/

.PHONY: all run run-validator clean test
