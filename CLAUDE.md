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

### Core Globals (src/globals.hpp)

- `g_console` - libtcod Console for rendering (80x50)
- `g_context` - libtcod Context for window/renderer management
- `g_state` - Current game state (polymorphic State pointer)
- `g_world` - Current world data (nullptr when at main menu)
- `g_controller` - Input controller (mouse cursor position)

### State Machine Pattern

The game uses a polymorphic state machine where each state handles its own input and rendering:

**Base State Interface** (`src/types/state.hpp`):
- `on_event(SDL_Event&)` - Handle input, return StateReturnType
- `on_draw()` - Render the current state

**State Results** (variant type in `src/types/state_result.hpp`):
- `Change{new_state}` - Transition to a new state
- `Reset{}` - Restart current state
- `EndTurn{}` - Player action complete, trigger enemy turns
- `Quit{}` - Exit game

**State Types**:
- `MainMenu` - Title screen (New Game, Continue, Exit)
- `InGame` - Main gameplay state
- `PickInventory` - Inventory selection overlay
- `PickTile` - Single-tile targeting cursor
- `PickTileAreaOfEffect` - AOE targeting with visual overlay
- `LevelUp` - Stat selection menu
- `Dead` - Death screen

**State Layering**: UI states like inventory/targeting draw the previous state in the background, then overlay their UI on top.

### SDL3 Callbacks

**SDL_AppInit** (`src/main.cpp`):
- Initializes libtcod console (80x50)
- Loads tileset from `data/dejavu16x16_gs_tc.png`
- Creates SDL3 window and renderer
- Initializes state machine with MainMenu

**SDL_AppIterate**:
- Clears console
- Calls `g_state->on_draw()` to render current state
- Presents console to screen
- Called every frame (~60fps)

**SDL_AppEvent**:
- Delegates event to `g_state->on_event()`
- Handles state transitions (Change, EndTurn, Quit)
- For EndTurn: updates FOV, runs enemy AI, checks level up/death
- Saves world on quit (ESC or window close)

**SDL_AppQuit**:
- Cleans up state machine
- SDL3 handles renderer/window cleanup automatically

### Game Systems

**World Structure** (`src/types/world.hpp`):
- `actors` - All actors (player + monsters) by ActorID
- `maps` - All generated maps by MapID
- `schedule` - Turn order for actors
- `log` - Message log with colors
- `rng` - Random number generator (std::mt19937)

**Map Structure** (`src/types/map.hpp`):
- `tiles` - 2D array of floor/wall tiles
- `explored` - Tiles player has seen
- `visible` - Tiles in current FOV
- `items` - Items on ground (Position -> Item)
- `fixtures` - Stairs, doors, etc.

**Action System** (`src/types/action.hpp`):
- All actions return variant Result (Success, Failure, Poll)
- Player actions: Bump (move/attack), Pickup, UseItem, UseStairs
- AI actions: BasicAI (A* pathfinding), ConfusedAI (random movement)
- Poll result creates new state (e.g., targeting UI)

**Pathfinding** (`src/pathfinding/`):
- Generic Pathfinder template supports A* and Dijkstra
- 2D grid graph with cost/flow maps
- Used by enemy AI to navigate around obstacles

**Procedural Generation** (`src/procgen/caves.hpp`):
- Cellular automata for cave generation
- Connected component labeling (hole filling)
- Random item/monster placement outside FOV
- Up/down stairs for level transitions

**Save/Load System** (`src/serialization.hpp`):
- JSON serialization for all game state
- Saves to `saves/save.json` on quit
- RNG state preserved for deterministic generation
- Backwards compatible migration support

### Data Directory

The `data/` folder contains assets (tilesets). The code searches parent directories for it at runtime via `get_data_dir()`.

## Dependencies (vcpkg.json)

- **libtcod** 2.2.1 - Roguelike library (with png, sdl, unicode, zlib features)
- **SDL3** 3.2.22 - Windowing and input (callback-based main loop)
- **fmt** 11.2.0 - Formatting library
- **nlohmann-json** 3.12.0 - JSON serialization for save/load system
- **ms-gsl** 4.2.0 - Microsoft Guidelines Support Library

Add new dependencies to `vcpkg.json` and link them in `CMakeLists.txt`.

## Directory Structure

```
src/
├── actions/          # Player/AI actions (bump, pickup, use_item, ai)
│   ├── ai_basic.hpp       # A* pathfinding AI
│   ├── ai_confused.hpp    # Confused status AI
│   ├── bump.hpp           # Movement and melee attack
│   ├── pickup.hpp         # Item pickup
│   ├── use_item.hpp       # Item usage
│   └── use_stairs.hpp     # Level transitions
├── items/            # Item types
│   ├── health_potion.hpp      # Healing consumable
│   ├── scroll_confusion.hpp   # Confusion debuff
│   ├── scroll_fireball.hpp    # AOE damage
│   └── scroll_lightning.hpp   # Auto-target damage
├── pathfinding/      # Pathfinding algorithms
│   ├── astar.hpp          # A* algorithm
│   ├── dijkstra.hpp       # Dijkstra's algorithm
│   ├── map.hpp            # 2D grid graph setup
│   └── pathfinding.hpp    # Generic pathfinder template
├── procgen/          # Procedural generation
│   └── caves.hpp          # Cellular automata cave generation
├── states/           # Game states
│   ├── dead.hpp               # Death screen
│   ├── ingame.hpp             # Main gameplay
│   ├── levelup.hpp            # Stat selection
│   ├── main_menu.cpp          # Main menu implementation
│   ├── main_menu.hpp          # Main menu declaration
│   ├── menu.hpp               # Base menu class
│   ├── pick_inventory.hpp     # Inventory UI
│   ├── pick_tile.hpp          # Single-tile targeting
│   └── pick_tile_aoe.hpp      # AOE targeting overlay
├── types/            # Core type definitions
│   ├── action.hpp         # Action base class
│   ├── action_result.hpp  # Action return types
│   ├── actor.hpp          # Actor (player/monster)
│   ├── actor_id.hpp       # Strongly-typed actor ID
│   ├── fixture.hpp        # Static map features
│   ├── item.hpp           # Item base class
│   ├── map.hpp            # Map structure
│   ├── map_id.hpp         # Strongly-typed map ID
│   ├── messages.hpp       # Message log
│   ├── ndarray.hpp        # Generic 2D array template
│   ├── position.hpp       # 2D position type
│   ├── state.hpp          # State base class
│   ├── state_result.hpp   # State return types
│   ├── stats.hpp          # Actor stats (HP, attack, etc.)
│   └── world.hpp          # World container
├── combat.hpp        # Combat calculations
├── constants.hpp     # Game constants (colors, sizes)
├── distance.hpp      # Distance metrics (Chebyshev, Euclidean)
├── errors.hpp        # Custom exceptions
├── fov.hpp           # Field of view calculations
├── globals.hpp       # Global state (console, context, world)
├── input_tools.hpp   # Input utilities
├── item_tools.hpp    # Item helper functions
├── json.hpp          # JSON configuration
├── main.cpp          # SDL3 entry point
├── maptools.hpp      # Map utility functions
├── rendering.hpp     # Rendering functions
├── serialization.hpp # Save/load system
├── world_init.hpp    # World initialization
├── world_logic.hpp   # Game logic (enemy turns)
└── xp.hpp            # Experience calculations
```

## Code Style

- Google C++ style base with 120 column limit
- 2-space indentation (4-space for CMakeLists.txt)
- Left-aligned pointers (`int* ptr` not `int *ptr`)
- Format on save enabled in VS Code via clang-format
