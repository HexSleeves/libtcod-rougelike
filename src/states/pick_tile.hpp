#pragma once
#include <cassert>

#include "../globals.hpp"
#include "../input_tools.hpp"
#include "../rendering.hpp"
#include "../types/state.hpp"

namespace state {
class PickTile : public State {
 public:
  using PickFunction = std::function<StateReturnType(GameContext&, Position)>;
  PickTile(std::unique_ptr<State> parent, const PickFunction& on_pick)
      : parent_{std::move(parent)}, on_pick_{on_pick} {};

  auto on_event(GameContext& context, SDL_Event& event) -> StateReturnType override {
    assert(context.world);
    if (const auto dir = get_dir_from(event); dir) {
      if (!context.controller.cursor) context.controller.cursor = context.world->active_player().pos;
      const auto& map = context.world->active_map();
      auto new_cursor = *context.controller.cursor + *dir;
      if (!map.tiles.in_bounds({new_cursor.x, context.controller.cursor->y}))
        new_cursor.x = context.controller.cursor->x;
      if (!map.tiles.in_bounds({context.controller.cursor->x, new_cursor.y}))
        new_cursor.y = context.controller.cursor->y;
      context.controller.cursor = new_cursor;
      return {};
    }

    switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
          case SDLK_RETURN:
          case SDLK_RETURN2:
          case SDLK_KP_ENTER:
            return on_pick(context);
          case SDLK_ESCAPE:
            return Change{std::move(parent_)};
          default:
            break;
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == 1) return on_pick(context);
        break;
      case SDL_EVENT_MOUSE_MOTION:
        context.context.convert_event_coordinates(event);
        context.controller.cursor = {static_cast<int>(event.motion.x), static_cast<int>(event.motion.y)};
        break;
      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        context.controller.cursor = std::nullopt;
        break;
      case SDL_EVENT_QUIT:
        return Quit{};
      default:
        break;
    }
    return {};
  }

  auto on_draw(GameContext& context) -> void override { render_all(context); }

 private:
  auto on_pick(GameContext& context) -> StateReturnType {
    if (!context.controller.cursor) return Change{std::move(parent_)};
    return on_pick_(context, *context.controller.cursor);
  }
  std::unique_ptr<State> parent_;
  PickFunction on_pick_;
};
}  // namespace state
