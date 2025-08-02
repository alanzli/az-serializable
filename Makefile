# Makefile for serialization framework
 CXX = g++
 HEADER_DIR = az
 INCLUDES = -I$(HEADER_DIR)
 CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(INCLUDES)
 BUILD_DIR = out
 EXAMPLE_DIR = examples
 TARGET = $(BUILD_DIR)/example
 SOURCES = $(EXAMPLE_DIR)/UsageExample.cpp

# Build directory target
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

 # Combined example target
 $(TARGET): $(BUILD_DIR) $(SOURCES) $(HEADER_DIR)/*.h
	 $(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

 # Build all
 all: $(TARGET)

 # Run combined example
 run: $(TARGET)
	 ./$(TARGET)

 # Clean build directory
 clean:
	 rm -rf $(BUILD_DIR)

 # Test combined implementation
 test: all
	 ./$(TARGET)

.PHONY: all run clean test
