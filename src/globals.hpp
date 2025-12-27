#pragma once
#include <libtcod.hpp>

#include "types/state.hpp"

// Phase 2: Core globals for state machine
// Phase 2: Core globals for state machine
// Moved to GameContext for better encapsulation
// inline tcod::Console g_console;
// inline tcod::Context g_context;
// inline std::unique_ptr<state::State> g_state;

// Phase 3: Additional globals
#include "types/controller.hpp"
#include "types/world.hpp"

// Encapsulates the entire game state
struct GameContext {
  tcod::Console console;
  tcod::Context context;
  tcod::Console log_console;  // Optimization: Pre-allocated console for logging
  std::unique_ptr<state::State> state;
  std::unique_ptr<World> world;
  Controller controller;
};
