cmake_minimum_required(VERSION 3.10)
project(az_serializable LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific compiler flags
if(MSVC)
    # MSVC specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2")
elseif(APPLE)
    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
elseif(UNIX)
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
endif()

# Directories
set(HEADER_DIR "${CMAKE_SOURCE_DIR}/az")
set(EXAMPLE_DIR "${CMAKE_SOURCE_DIR}/examples")

include_directories(${CMAKE_SOURCE_DIR})

# Example targets
add_executable(example ${EXAMPLE_DIR}/UsageExample.cpp)
add_executable(validator_example ${EXAMPLE_DIR}/ValidatorExample.cpp)
add_executable(auto_serializable_example ${EXAMPLE_DIR}/AutoSerializableExample.cpp)
