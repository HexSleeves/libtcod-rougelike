cmake_minimum_required(VERSION 4.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(tcod-adventure LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

find_package(SDL3 CONFIG REQUIRED)
find_package(libtcod CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

file(
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS # Automatically reconfigure if source files are added/removed.
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.hpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

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
    <fmt/format.h>
    <libtcod.hpp>
    <SDL3/SDL.h>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    libtcod::libtcod
    fmt::fmt
    nlohmann_json::nlohmann_json
    Microsoft.GSL::GSL
)

if(EMSCRIPTEN)
    target_link_options(${PROJECT_NAME} PRIVATE
        --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/data@data")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()
