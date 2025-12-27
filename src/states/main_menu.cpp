#include "main_menu.hpp"

#include <SDL3/SDL.h>

#include <iostream>

namespace state {
MainMenu::MainMenu(int selected)
    : Menu{
          MenuItems{
              {"[N] New Game",
               []() -> state::Result {
                 // Phase 3: Will initialize world and change to InGame state
                 std::cout << "New Game selected (Phase 3 placeholder)\n";
                 return {};
               },
               SDLK_N},  // SDL3: uppercase key codes
              {"[C] Continue",
               []() -> state::Result {
                 // Phase 3: Will load saved game or resume current game
                 std::cout << "Continue selected (Phase 3 placeholder)\n";
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
