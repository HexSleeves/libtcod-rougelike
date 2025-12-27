#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <libtcod.hpp>
#include <variant>

// Phase 1: Core types
#include "constants.hpp"
#include "errors.hpp"
#include "types/actor_id.hpp"
#include "types/fixture.hpp"
#include "types/item.hpp"
#include "types/map_id.hpp"
#include "types/ndarray.hpp"
#include "types/position.hpp"
#include "types/stats.hpp"

// Phase 2: State machine
#include "globals.hpp"
#include "states/main_menu.hpp"
#include "types/state.hpp"
#include "types/state_result.hpp"

// Phase 4: Combat and gameplay
#include "fov.hpp"
#include "world_logic.hpp"
#include "xp.hpp"

// Phase 5: Additional states
#include "states/dead.hpp"
#include "states/ingame.hpp"
#include "states/levelup.hpp"

// Phase 6: Serialization
#include "serialization.hpp"

/// Return the data directory.
auto get_data_dir() -> std::filesystem::path {
  static auto root_directory = std::filesystem::path{"."};  // Begin at the working directory.
  while (!std::filesystem::exists(root_directory / "data")) {
    // If the current working directory is missing the data dir then it will assume it exists in any parent directory.
    root_directory /= "..";
    if (!std::filesystem::exists(root_directory)) {
      throw DataDirectoryNotFoundError("Could not find the data directory.");
    }
  }
  return root_directory / "data";
};

// Called every frame - render current state
SDL_AppResult SDL_AppIterate(void*) {
  g_console.clear();
  if (g_state) {
    g_state->on_draw();
  }
  g_context.present(g_console);
  return SDL_APP_CONTINUE;
}

// Handle events - delegate to current state
SDL_AppResult SDL_AppEvent(void*, SDL_Event* event) {
  if (!g_state) return SDL_APP_CONTINUE;

  // Let the current state handle the event and handle state transitions
  if (auto result = g_state->on_event(*event); std::holds_alternative<state::Change>(result)) {
    g_state = std::move(std::get<state::Change>(result).new_state);
  } else if (std::holds_alternative<state::Quit>(result)) {
    if (g_world) save_world(*g_world);
    return SDL_APP_SUCCESS;
  } else if (std::holds_alternative<state::EndTurn>(result)) {
    // Phase 4: Handle enemy turns after player action
    g_controller.cursor = std::nullopt;

    if (g_world && g_world->active_player().stats.hp > 0) {
      auto& world = *g_world;
      update_fov(world.active_map(), world.active_player().pos);
      enemy_turn(world);

      // Check for level up
      if (g_world->active_player().stats.xp >= next_level_xp(g_world->active_player().stats.level)) {
        g_state = std::make_unique<state::LevelUp>();
      } else {
        g_state = std::make_unique<state::InGame>();
      }
    } else if (g_world && g_world->active_player().stats.hp <= 0) {
      // Player died
      g_state = std::make_unique<state::Dead>();
    }
  }

  // Also handle SDL_EVENT_QUIT
  if (event->type == SDL_EVENT_QUIT) {
    if (g_world) save_world(*g_world);
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

// Main entry point
SDL_AppResult SDL_AppInit(void**, int argc, char** argv) {
  auto params = TCOD_ContextParams{};
  params.argc = argc;
  params.argv = argv;
  params.renderer_type = TCOD_RENDERER_SDL2;
  params.vsync = 1;
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.window_title = "Libtcod Roguelike";

  auto tileset = tcod::load_tilesheet(get_data_dir() / "dejavu16x16_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
  params.tileset = tileset.get();

  // Initialize console with constants
  g_console = tcod::Console{constants::CONSOLE_WIDTH, constants::CONSOLE_HEIGHT};
  params.console = g_console.get();

  g_context = tcod::Context(params);

  // Phase 2: Initialize state machine
  std::cout << "Phase 2: Initializing state machine with MainMenu...\n";
  g_state = std::make_unique<state::MainMenu>();
  std::cout << "State machine initialized!\n\n";

  return SDL_APP_CONTINUE;
}

// Called before exiting
void SDL_AppQuit(void*, SDL_AppResult) {
  std::cout << "Shutting down...\n";
  g_state.reset();  // Clean up state
}
