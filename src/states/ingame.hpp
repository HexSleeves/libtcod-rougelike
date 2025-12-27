#pragma once
#include <SDL3/SDL.h>

#include <cassert>

#include "../actions/bump.hpp"
#include "../actions/pickup.hpp"
#include "../actions/use_item.hpp"
#include "../actions/use_stairs.hpp"
#include "../globals.hpp"
#include "../input_tools.hpp"
#include "../procgen/caves.hpp"
#include "../rendering.hpp"
#include "../serialization.hpp"
#include "../types/state.hpp"
#include "main_menu.hpp"
#include "pick_inventory.hpp"

namespace state {
class InGame : public State {
 public:
  auto on_draw() -> void override { render_all(g_console, *g_world); }

  auto on_event(SDL_Event& event) -> StateReturnType override {
    assert(g_world);
    assert(g_world->schedule.front() == ActorID{0});

    auto& world = *g_world;

    switch (event.type) {
      case SDL_EVENT_KEY_DOWN: {
        switch (event.key.key) {
          case SDLK_G:
            return do_action(action::Pickup{});
          case SDLK_I:
            return Change{std::make_unique<PickInventory>(
                std::move(g_state), [](auto item_index) { return do_action(action::UseItem{item_index}); })};
          case SDLK_COMMA:
            if (event.key.mod & SDL_KMOD_SHIFT) return do_action(action::UseStairs(false));
            break;
          case SDLK_PERIOD:
            if (event.key.mod & SDL_KMOD_SHIFT) return do_action(action::UseStairs(true));
            break;
          case SDLK_F2:
            procgen::generate_level(world);
            return {};
          case SDLK_F3:
            for (auto&& it : world.active_map().explored) it = true;
            return {};
          case SDLK_ESCAPE:
            save_world(world);
            return Change{std::make_unique<MainMenu>()};
          default:
            break;
        }
        if (auto dir = get_dir_from(event); dir) {
          return cmd_move(*dir);
        }
      } break;
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

 private:
  static auto cmd_move(Position dir) -> StateReturnType { return do_action(action::Bump{dir}); }
  static auto do_action(action::Action& my_action) -> StateReturnType {
    auto& world = *g_world;
    return after_action(my_action.perform(world, world.active_player()));
  }

  static auto do_action(action::Action&& my_action) -> StateReturnType {
    auto& world = *g_world;
    return after_action(my_action.perform(world, world.active_player()));
  }

  static auto after_action(action::Result result) -> StateReturnType {
    if (std::holds_alternative<action::Failure>(result)) {
      g_world->log.append(std::get<action::Failure>(result).reason);
      return Change{std::make_unique<InGame>()};
    } else if (std::holds_alternative<action::Poll>(result)) {
      return Change{std::move(std::get<action::Poll>(result).new_state)};
    } else if (std::holds_alternative<action::Success>(result)) {
      return EndTurn{};
    } else {
      assert(0);
    }
    return Change{std::make_unique<InGame>()};
  }
};
}  // namespace state
