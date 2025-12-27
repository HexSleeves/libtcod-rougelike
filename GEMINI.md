# Libtcod Roguelike Template

## Project Overview

This is a C++ template project for developing roguelike games using the `libtcod` library and `SDL3`. It is configured to use `CMake` for the build system and `vcpkg` for package management, making it cross-platform and easy to set up.

The project is set up as a "Hello World" application where a player character (`@`) can be moved around a screen using arrow keys or Vi-keys.

## Architecture & Technologies

* **Language:** C++ (C++17 standard)
* **Build System:** CMake
* **Dependency Management:** vcpkg (included as a git submodule)
* **Libraries:**
  * `libtcod`: Roguelike engine (FOV, pathfinding, consoles, etc.)
  * `SDL3`: Low-level access to audio, keyboard, mouse, joystick, and graphics hardware.
  * `fmt`: A modern formatting library.

## Key Files

* **`src/main.cpp`**: The main entry point of the application. It uses the SDL3 application callback structure (`SDL_AppInit`, `SDL_AppIterate`, `SDL_AppEvent`, `SDL_AppQuit`) instead of a traditional `main` loop.
* **`CMakeLists.txt`**: The CMake configuration file. It sets up the project name, sources, compiler options, and links dependencies via `find_package`.
* **`vcpkg.json`**: The manifest file listing the project's dependencies (`libtcod`, `sdl3`, `fmt`).
* **`data/`**: Directory containing runtime assets. Currently holds the font file `dejavu16x16_gs_tc.png`.

## Building and Running

### Prerequisites

* **CMake**: 3.19 or later.
* **C++ Compiler**: GCC, Clang, or MSVC supporting C++17.
* **Git**: For submodule management.

### Setup

1. **Initialize Submodules**:
    The project relies on `vcpkg` as a submodule. You **must** initialize it before building.

    ```bash
    git submodule update --init
    ```

2. **Build with CMake**:
    You can use the command line or an IDE like VS Code (recommended).

    **Command Line:**

    ```bash
    # Configure (this will download and build vcpkg dependencies)
    cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake

    # Build
    cmake --build build

    # Run
    ./build/bin/tcod-adventure
    ```

    **VS Code:**
    * Install the "CMake Tools" extension.
    * Open the folder in VS Code.
    * Select a kit (compiler).
    * Press F5 or click "Build" / "Run" in the status bar.

## Development Conventions

* **Code Style**: The project includes `.clang-format` and `.editorconfig` files. Adhere to these styles.
* **Source Structure**: Place source files (`.cpp`) and headers (`.hpp`) in the `src/` directory. `CMakeLists.txt` is configured to glob these files automatically.
* **Assets**: Place game assets in `data/`. The code includes a helper to locate this directory.
* **Dependencies**: To add new libraries, modify `vcpkg.json` and update `CMakeLists.txt` to find and link them.

## Troubleshooting

* **Missing Dependencies**: If the build fails due to missing libraries, ensure you have run `git submodule update --init`.
* **Data Directory Not Found**: The application looks for the `data/` folder in the current or parent directories. Ensure you are running the executable from a location where it can find `data/`.
