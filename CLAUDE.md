# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A C++17 roguelike game built with libtcod and SDL3. Uses CMake with vcpkg (as a Git submodule) for dependency management.

## Build Commands

```bash
# Initialize vcpkg submodule (required before first build)
git submodule update --init

# Configure (from project root)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run executable
./build/bin/tcod-adventure
```

### Platform-Specific Notes

**Linux** - Install these dependencies before building:

```bash
sudo apt-get install libltdl-dev libx11-dev libxft-dev libxext-dev libwayland-dev libxkbcommon-dev libegl1-mesa-dev libibus-1.0-dev
```

**Windows** - Use Visual Studio Developer Command Prompt or ensure MSVC toolchain is available.

**Emscripten (WebAssembly):**

```bash
emcmake cmake -S . -B build -G Ninja \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=wasm32-emscripten
cmake --build build
```

## Pre-commit Hooks

```bash
pip install pre-commit
pre-commit install
```

Runs clang-format and various file checks automatically on commit.

## Architecture

### Entry Point Pattern

Uses SDL3's callback-based main (`SDL_MAIN_USE_CALLBACKS`):

- `SDL_AppInit` - Initialize console, context, tileset
- `SDL_AppIterate` - Main game loop (renders each frame)
- `SDL_AppEvent` - Event handling (keyboard input)
- `SDL_AppQuit` - Cleanup

### Core Globals (src/main.cpp)

- `g_console` - libtcod Console for rendering
- `g_context` - libtcod Context for window/renderer management

### Data Directory

The `data/` folder contains assets (tilesets). The code searches parent directories for it at runtime via `get_data_dir()`.

## Dependencies (vcpkg.json)

- **libtcod** - Roguelike library (with png, sdl, unicode, zlib features)
- **SDL3** - Windowing and input
- **fmt** - Formatting library (available for use)

Add new dependencies to `vcpkg.json` and link them in `CMakeLists.txt`.

## Code Style

- Google C++ style base with 120 column limit
- 2-space indentation (4-space for CMakeLists.txt)
- Left-aligned pointers (`int* ptr` not `int *ptr`)
- Format on save enabled in VS Code via clang-format
