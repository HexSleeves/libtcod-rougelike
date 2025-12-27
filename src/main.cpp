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

  // Let the current state handle the event
  auto result = g_state->on_event(*event);

  // Handle state transitions
  if (std::holds_alternative<state::Change>(result)) {
    g_state = std::move(std::get<state::Change>(result).new_state);
  } else if (std::holds_alternative<state::Quit>(result)) {
    return SDL_APP_SUCCESS;
  }
  // Handle other result types (Reset, EndTurn) in future phases

  // Also handle SDL_EVENT_QUIT
  if (event->type == SDL_EVENT_QUIT) {
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
