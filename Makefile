# Makefile for serialization framework
ifeq ($(shell uname), Darwin)
    CXX = clang++
else
    CXX = g++
endif
HEADER_DIR = az
INCLUDES = -I. -I$(HEADER_DIR)
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(INCLUDES)
BUILD_DIR = out
EXAMPLE_DIR = examples
TARGET = $(BUILD_DIR)/example
VALIDATOR_TARGET = $(BUILD_DIR)/validator_example
SOURCES = $(EXAMPLE_DIR)/UsageExample.cpp
VALIDATOR_SOURCES = $(EXAMPLE_DIR)/ValidatorExample.cpp

# Build directory target
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

 # Combined example target
 $(TARGET): $(BUILD_DIR) $(SOURCES) $(HEADER_DIR)/*.h
	 $(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

 # Validator example target
 $(VALIDATOR_TARGET): $(BUILD_DIR) $(VALIDATOR_SOURCES) $(HEADER_DIR)/*.h
	 $(CXX) $(CXXFLAGS) -o $(VALIDATOR_TARGET) $(VALIDATOR_SOURCES)

 # Build all
 all: $(TARGET) $(VALIDATOR_TARGET)

 # Run original example
 run: $(TARGET)
	 ./$(TARGET)

 # Run validator example
 run-validator: $(VALIDATOR_TARGET)
	 ./$(VALIDATOR_TARGET)


 # Clean build directory
 clean:
	 rm -rf $(BUILD_DIR)

 # Test implementation
 test: all
	 echo "Running basic serialization example..."
	 ./$(TARGET)
	 echo "Running validator example..."
	 ./$(VALIDATOR_TARGET)

.PHONY: all run run-validator run-enhanced-validator clean test
