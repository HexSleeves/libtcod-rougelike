#include "main_menu.hpp"

#include <SDL3/SDL.h>

#include <iostream>
#include <memory>

#include "../globals.hpp"
#include "../world_init.hpp"
#include "ingame.hpp"

namespace state {
MainMenu::MainMenu(int selected)
    : Menu{
          MenuItems{
              {"[N] New Game",
               []() -> state::Result {
                 g_world = new_world();
                 return state::Change{std::make_unique<state::InGame>()};
               },
               SDLK_N},  // SDL3: uppercase key codes
              {"[C] Continue",
               []() -> state::Result {
                 // Phase 6: Will load saved game
                 if (g_world) return state::Change{std::make_unique<state::InGame>()};
                 return {};
               },
               SDLK_C},  // SDL3: uppercase key codes
#ifndef __EMSCRIPTEN__
              {"[Q] Exit", []() -> state::Result { return Quit{}; }, SDLK_Q},  // SDL3: uppercase key codes
#endif
          },
          selected} {
}
}  // namespace state
