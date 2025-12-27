#include "main_menu.hpp"

#include <SDL3/SDL.h>

#include <memory>

#include "../globals.hpp"
#include "../serialization.hpp"
#include "../world_init.hpp"
#include "ingame.hpp"

namespace state {
MainMenu::MainMenu(int selected)
    : Menu{
          MenuItems{
              {"[N] New Game",
               [](GameContext& context) -> state::Result {
                 context.world = new_world();
                 return state::Change{std::make_unique<state::InGame>()};
               },
               SDLK_N},  // SDL3: uppercase key codes
              {"[C] Continue",
               [](GameContext& context) -> state::Result {
                 std::unique_ptr<World> loaded = load_world();
                 if (loaded) {
                   context.world = std::move(loaded);
                   return state::Change{std::make_unique<state::InGame>()};
                 }
                 return {};
               },
               SDLK_C},  // SDL3: uppercase key codes
#ifndef __EMSCRIPTEN__
              {"[Q] Exit", [](GameContext&) -> state::Result { return Quit{}; }, SDLK_Q},  // SDL3: uppercase key codes
#endif
          },
          selected} {
}
}  // namespace state
