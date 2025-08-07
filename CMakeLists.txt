cmake_minimum_required(VERSION 3.14)
#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
project(NeoGUI VERSION "0.0.1")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force g++14 as g++13 does not implement some C++20 features
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_COMPILER "g++-14")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(DIR_SEPARATOR="\\\\")
else()
    add_compile_definitions(DIR_SEPARATOR="/")
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/src/Version.h.in
    ${CMAKE_BINARY_DIR}/Version.h
)

# set DEBUG mode
if (DEBUG)
    add_compile_definitions(
        DEBUG=1
    )
    message(STATUS "DEBUG mode enabled")
endif()

# Add SDK include directory
include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/sdk
    ${CMAKE_SOURCE_DIR}/src
)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
)

FetchContent_MakeAvailable(SFML)

# Source files
# To set after starting development
set(SOURCES
    "src/NeoGUI.cpp"
    src/main.cpp
)

# Define the plugin library
add_library(${PROJECT_NAME} SHARED ${SOURCES}    "src/config/Colors.h")

#set_property(TARGET ${PROJECT_NAME}  PROPERTY CXX_STANDARD 20)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json sfml-graphics sfml-window sfml-system)

# Set output directory and properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PREFIX ""  # Remove "lib" prefix on Unix-like systems
)

# Add processor-specific output name for Apple platforms
if(${CMAKE_HOST_APPLE})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}
    )
endif()

# Copy them to the build directory
file(COPY ${CONFIG_JSON_FILES} DESTINATION ${CMAKE_BINARY_DIR})

