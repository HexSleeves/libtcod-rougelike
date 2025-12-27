#pragma once
#include <cassert>

#include "../globals.hpp"
#include "../input_tools.hpp"
#include "../rendering.hpp"
#include "../types/state.hpp"

namespace state {
class PickTile : public State {
 public:
  using PickFunction = std::function<StateReturnType(Position)>;
  PickTile(std::unique_ptr<State> parent, const PickFunction& on_pick)
      : parent_{std::move(parent)}, on_pick_{on_pick} {};

  auto on_event(SDL_Event& event) -> StateReturnType override {
    assert(g_world);
    if (const auto dir = get_dir_from(event); dir) {
      if (!g_controller.cursor) g_controller.cursor = g_world->active_player().pos;
      const auto& map = g_world->active_map();
      auto new_cursor = *g_controller.cursor + *dir;
      if (!map.tiles.in_bounds({new_cursor.x, g_controller.cursor->y})) new_cursor.x = g_controller.cursor->x;
      if (!map.tiles.in_bounds({g_controller.cursor->x, new_cursor.y})) new_cursor.y = g_controller.cursor->y;
      g_controller.cursor = new_cursor;
      return {};
    }

    switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
          case SDLK_RETURN:
          case SDLK_RETURN2:
          case SDLK_KP_ENTER:
            return on_pick();
          case SDLK_ESCAPE:
            return Change{std::move(parent_)};
          default:
            break;
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == 1) return on_pick();
        break;
      case SDL_EVENT_MOUSE_MOTION:
        g_context.convert_event_coordinates(event);
        g_controller.cursor = {static_cast<int>(event.motion.x), static_cast<int>(event.motion.y)};
        break;
      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        g_controller.cursor = std::nullopt;
        break;
      case SDL_EVENT_QUIT:
        return Quit{};
      default:
        break;
    }
    return {};
  }

  auto on_draw() -> void override { render_all(g_console, *g_world); }

 private:
  auto on_pick() -> StateReturnType {
    if (!g_controller.cursor) return Change{std::move(parent_)};
    return on_pick_(*g_controller.cursor);
  }
  std::unique_ptr<State> parent_;
  PickFunction on_pick_;
};
}  // namespace state
