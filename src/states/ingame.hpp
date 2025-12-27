#pragma once
#include <SDL3/SDL.h>

#include "../globals.hpp"
#include "../rendering.hpp"
#include "../types/state.hpp"

namespace state {
class InGame : public State {
 public:
  auto on_event(SDL_Event& event) -> StateReturnType override {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        return Quit{};
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
          case SDLK_ESCAPE:
            return Quit{};
          // Phase 4: Will add movement and actions
          default:
            break;
        }
        break;
      default:
        break;
    }
    return {};
  }

  auto on_draw() -> void override {
    if (g_world) {
      render_all(g_console, *g_world);
    }
  }
};
}  // namespace state
