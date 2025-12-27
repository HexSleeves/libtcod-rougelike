#pragma once
#include <SDL3/SDL_events.h>

#include <optional>

#include "types/position.hpp"

/// Return a direction that this event corresponds to.
inline auto get_dir_from(const SDL_Event& event) -> std::optional<Position> {
  switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
      const int modified_y = (event.key.mod & SDL_KMOD_SHIFT ? -1 : 0) + (event.key.mod & SDL_KMOD_CTRL ? 1 : 0);
      switch (event.key.key) {
        case SDLK_PERIOD:
        case SDLK_KP_5:
          return Position{0, 0};
        case SDLK_UP:
          return Position{0, -1};
        case SDLK_DOWN:
          return Position{0, 1};
        case SDLK_LEFT:
          return Position{-1, modified_y};
        case SDLK_RIGHT:
          return Position{1, modified_y};
        case SDLK_HOME:
          return Position{-1, -1};
        case SDLK_END:
          return Position{-1, 1};
        case SDLK_PAGEUP:
          return Position{1, -1};
        case SDLK_PAGEDOWN:
          return Position{1, 1};
        case SDLK_KP_1:
        case SDLK_B:
          return Position{-1, 1};
        case SDLK_KP_2:
        case SDLK_J:
          return Position{0, 1};
        case SDLK_KP_3:
        case SDLK_N:
          return Position{1, 1};
        case SDLK_KP_4:
        case SDLK_H:
          return Position{-1, 0};
        case SDLK_KP_6:
        case SDLK_L:
          return Position{1, 0};
        case SDLK_KP_7:
        case SDLK_Y:
          return Position{-1, -1};
        case SDLK_KP_8:
        case SDLK_K:
          return Position{0, -1};
        case SDLK_KP_9:
        case SDLK_U:
          return Position{1, -1};
        default:
          break;
      }
    }
    default:
      break;
  }
  return {};
}
