# AGENTS.md - Agentic Coding Guidelines

This document provides guidelines for AI coding agents working in this C++17 libtcod roguelike project.

## Project Overview

- **Language**: C++17
- **Build System**: CMake (3.19+)
- **Package Manager**: vcpkg (Git submodule)
- **Dependencies**: libtcod, SDL3, fmt
- **Target Platforms**: Windows, macOS, Linux, WebAssembly (Emscripten)

## Build Commands

### Prerequisites

```bash
# Initialize vcpkg submodule (required before first build)
git submodule update --init
```

### Configure and Build

```bash
# Configure (Debug)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Configure (Release)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Build with specific target
cmake --build build --target tcod-adventure
```

### Platform-Specific Builds

```bash
# macOS (arm64)
cmake -S . -B build -DVCPKG_TARGET_TRIPLET=arm64-osx -DCMAKE_BUILD_TYPE=Release

# Linux (x64)
cmake -S . -B build -DVCPKG_TARGET_TRIPLET=x64-linux -DCMAKE_BUILD_TYPE=Release

# Windows (x64)
cmake -S . -B build -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Release

# Emscripten/WebAssembly
emcmake cmake -S . -B build -G Ninja \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=wasm32-emscripten
```

### Run the Application

```bash
# Executable location after build
./build/bin/tcod-adventure
```

## Testing

**Note**: This project currently has no test framework configured. If adding tests:

- Consider using Catch2 or Google Test (available via vcpkg)
- Add test files to `src/` or create a `tests/` directory
- Update CMakeLists.txt to include test targets

## Linting and Formatting

### Pre-commit Hooks (Required)

```bash
# Install pre-commit
pip install pre-commit

# Enable hooks
pre-commit install

# Run manually on all files
pre-commit run --all-files

# Run specific hook
pre-commit run clang-format --all-files
```

### clang-format

The project uses clang-format with a Google-based style. Format manually:

```bash
clang-format -i src/*.cpp src/*.hpp
```

## Code Style Guidelines

### Formatting (from .clang-format)

- **Base Style**: Google
- **Indent Width**: 2 spaces
- **Column Limit**: 120 characters
- **Pointer Alignment**: Left (`int* ptr` not `int *ptr`)
- **Brace Breaking**: Always break after open bracket for function arguments
- **Bin Packing**: Disabled (one argument per line when wrapped)

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Functions | snake_case | `get_data_dir()` |
| Variables | snake_case | `player_x`, `root_directory` |
| Constants | UPPER_SNAKE or PascalCase | `WHITE`, `SDL_APP_CONTINUE` |
| Global variables | g_ prefix | `g_console`, `g_context` |
| Types/Classes | PascalCase | `ColorRGB`, `Console` |
| Namespaces | lowercase | `tcod`, `std` |

### Import/Include Order

1. Main header (if .cpp has corresponding .hpp)
2. SDL headers (`<SDL3/SDL.h>`)
3. Standard library (`<cstdlib>`, `<filesystem>`)
4. Third-party libraries (`<libtcod.hpp>`, `<fmt/core.h>`)
5. Project headers

### Type Conventions

- Use `auto` for complex return types and iterators
- Prefer trailing return type syntax: `auto func() -> ReturnType`
- Use `constexpr` for compile-time constants
- Use `static constexpr` for file-scope constants
- Use `std::nullopt` instead of empty optionals

### Error Handling

- Throw `std::runtime_error` for fatal errors
- Use exceptions for initialization failures
- SDL callbacks should return appropriate `SDL_AppResult` values

### Modern C++ Practices

- Use `std::filesystem` for path operations
- Use aggregate initialization: `TCOD_ContextParams{}`
- Prefer `{}` initialization over `()`
- Use range-based for loops

## Project Structure

```
libtcod-rougelike/
├── src/                    # Source files (*.cpp, *.hpp)
│   └── main.cpp           # Entry point with SDL callbacks
├── data/                   # Game assets (tilesets, etc.)
├── build/                  # Build output (gitignored)
├── vcpkg/                  # Package manager (submodule)
├── CMakeLists.txt         # Build configuration
├── vcpkg.json             # Dependencies manifest
├── .clang-format          # Code formatting rules
├── .editorconfig          # Editor settings
└── .pre-commit-config.yaml # Git hooks
```

## Adding Dependencies

1. Find the package on [vcpkg.info](https://vcpkg.info/)
2. Add to `vcpkg.json`:

   ```json
   "dependencies": ["new-package"]
   ```

3. Add to `CMakeLists.txt`:

   ```cmake
   find_package(new-package CONFIG REQUIRED)
   target_link_libraries(${PROJECT_NAME} PRIVATE new-package::new-package)
   ```

## SDL3 Callback Architecture

This project uses SDL3's callback-based main loop:

```cpp
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void**, int argc, char** argv);  // Initialization
SDL_AppResult SDL_AppIterate(void*);                        // Main loop (every frame)
SDL_AppResult SDL_AppEvent(void*, SDL_Event* event);        // Event handling
void SDL_AppQuit(void*, SDL_AppResult);                     // Cleanup
```

Return values:

- `SDL_APP_CONTINUE` - Keep running
- `SDL_APP_SUCCESS` - Exit successfully
- `SDL_APP_FAILURE` - Exit with error

## CI/CD

GitHub Actions workflow (`.github/workflows/cmake.yml`) builds for:

- Windows (x64-windows)
- macOS (arm64-osx)
- Ubuntu Linux (x64-linux)
- WebAssembly (wasm32-emscripten)

Artifacts are uploaded on successful builds. Tagged commits create GitHub Releases.

## Common Issues

### vcpkg Submodule Empty

```bash
git submodule update --init
```

### Data Directory Not Found

The application searches parent directories for `data/`. Ensure you run from project root or a subdirectory.

### Compiler Warnings

Project enables `-Wall -Wextra` (GCC/Clang) or `/W4` (MSVC). Fix all warnings before committing.

## Key Libraries Reference

- **libtcod**: [libtcod Documentation](https://libtcod.readthedocs.io/)
- **SDL3**: [SDL3 Wiki](https://wiki.libsdl.org/SDL3)
- **fmt**: [fmt Documentation](https://fmt.dev/)
