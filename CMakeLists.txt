cmake_minimum_required (VERSION 3.19...4.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(
    tcod-adventure  # Project name, change this as needed.
    LANGUAGES C CXX
)

# Generate compile_commands.json for static analysis tools (SonarQube, clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")  # Keep all runtime files in one directory.

file(
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS  # Automatically reconfigure if source files are added/removed.
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.hpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Precompiled headers for faster compilation
target_precompile_headers(${PROJECT_NAME} PRIVATE
    <algorithm>
    <array>
    <cmath>
    <filesystem>
    <iostream>
    <memory>
    <optional>
    <random>
    <string>
    <tuple>
    <variant>
    <vector>
    <fmt/core.h>
    <fmt/format.h>
    <libtcod.hpp>
    <SDL3/SDL.h>
)

# Ensure the C++17 standard is available.
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Link filesystem library for C++17 std::filesystem support
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Some Clang setups may need explicit filesystem linking
    if (NOT APPLE)
        target_link_libraries(${PROJECT_NAME} PRIVATE c++fs)
    endif()
endif()

# Enforce UTF-8 encoding on MSVC.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# Enable warnings recommended for new projects.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

if (EMSCRIPTEN)
    # Attach data folder to Emscripten builds.
    target_link_options(${PROJECT_NAME} PRIVATE --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/data@data")
    # Set output to html to generate preview pages, otherwise you'll have to make your own html.
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            SUFFIX ".html"
    )
endif()

find_package(SDL3 CONFIG REQUIRED)
find_package(libtcod CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        SDL3::SDL3
        libtcod::libtcod
        fmt::fmt
        nlohmann_json::nlohmann_json
        Microsoft.GSL::GSL
)
