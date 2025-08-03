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
set(DIST_DIR "${CMAKE_BINARY_DIR}/dist")

include_directories(${CMAKE_SOURCE_DIR})

# Example targets
add_executable(example ${EXAMPLE_DIR}/UsageExample.cpp)
add_executable(validator_example ${EXAMPLE_DIR}/ValidatorExample.cpp)

# Custom targets for running examples
if(WIN32)
    add_custom_target(run
        COMMAND example.exe
        DEPENDS example
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_target(run-validator
        COMMAND validator_example.exe
        DEPENDS validator_example
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    add_custom_target(run
        COMMAND example
        DEPENDS example
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_target(run-validator
        COMMAND validator_example
        DEPENDS validator_example
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Clean target
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DIST_DIR}")

# Dist target
add_custom_target(dist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${HEADER_DIR} ${DIST_DIR}/az
)
