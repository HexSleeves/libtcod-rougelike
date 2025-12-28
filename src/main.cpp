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
SDL_AppResult SDL_AppIterate(void* appstate) {
  auto* app = static_cast<GameContext*>(appstate);
  app->console.clear();
  if (app->state) {
    app->state->on_draw(*app);
  }
  app->context.present(app->console);
  return SDL_APP_CONTINUE;
}

// Handle events - delegate to current state
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  auto* app = static_cast<GameContext*>(appstate);
  if (!app->state) return SDL_APP_CONTINUE;

  // Let the current state handle the event and handle state transitions
  if (auto result = app->state->on_event(*app, *event); std::holds_alternative<state::Change>(result)) {
    app->state = std::move(std::get<state::Change>(result).new_state);
  } else if (std::holds_alternative<state::Quit>(result)) {
    if (app->world) save_world(*app->world);
    return SDL_APP_SUCCESS;
  } else if (std::holds_alternative<state::EndTurn>(result)) {
    // Phase 4: Handle enemy turns after player action
    app->controller.cursor = std::nullopt;

    if (app->world && app->world->active_player().stats.hp > 0) {
      auto& world = *app->world;
      update_fov(world.active_map(), world.active_player().pos);
      enemy_turn(*app);

      // Check for level up
      if (app->world->active_player().stats.xp >= next_level_xp(app->world->active_player().stats.level)) {
        app->state = std::make_unique<state::LevelUp>(*app);
      } else {
        // Optimization: Keep the current InGame state active.
        // app->state = std::make_unique<state::InGame>();
      }
    } else if (app->world && app->world->active_player().stats.hp <= 0) {
      // Player died
      app->state = std::make_unique<state::Dead>();
    }
  } else if (std::holds_alternative<state::Reset>(result)) {
    // Reset state (e.g. after LevelUp) -> Go back to InGame usually or stick to current?
    // LevelUp returns Reset when done.
    app->state = std::make_unique<state::InGame>();
  }

  // Also handle SDL_EVENT_QUIT
  if (event->type == SDL_EVENT_QUIT) {
    if (app->world) save_world(*app->world);
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

// Main entry point
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  auto params = TCOD_ContextParams{};
  params.argc = argc;
  params.argv = argv;
  params.renderer_type = TCOD_RENDERER_SDL2;
  params.vsync = 1;
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.window_title = "Libtcod Roguelike";

  auto tileset = tcod::load_tilesheet(get_data_dir() / "dejavu16x16_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
  params.tileset = tileset.get();

  auto* app = new GameContext();
  *appstate = app;

  // Initialize console with constants
  app->console = tcod::Console{constants::CONSOLE_WIDTH, constants::CONSOLE_HEIGHT};
  params.console = app->console.get();

  app->context = tcod::Context(params);
  app->state = std::make_unique<state::MainMenu>();

  return SDL_APP_CONTINUE;
}

// Called before exiting
void SDL_AppQuit(void* appstate, SDL_AppResult) {
  std::cout << "Shutting down...\n";
  auto* app = static_cast<GameContext*>(appstate);
  delete app;
}
