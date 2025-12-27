#pragma once
#include <libtcod.hpp>

#include "types/state.hpp"

// Phase 2: Core globals for state machine
inline tcod::Console g_console;  // The global console object.
inline tcod::Context g_context;  // The global libtcod context.
inline std::unique_ptr<state::State> g_state;

// Phase 3: Additional globals (to be uncommented)
// #include "types/controller.hpp"
// #include "types/world.hpp"
// inline std::unique_ptr<World> g_world;  // The active world object.
// inline Controller g_controller;
