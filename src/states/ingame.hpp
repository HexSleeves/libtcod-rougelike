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
  auto on_draw(GameContext& context) -> void override { render_all(context); }

  auto on_event(GameContext& context, SDL_Event& event) -> StateReturnType override {
    assert(context.world);
    assert(context.world->schedule.front() == ActorID{0});

    auto& world = *context.world;

    switch (event.type) {
      case SDL_EVENT_KEY_DOWN: {
        switch (event.key.key) {
          case SDLK_G:
            return do_action(context, action::Pickup{});
          case SDLK_I:
            return Change{std::make_unique<PickInventory>(
                std::move(context.state),
                [](GameContext& ctx, int item_index) { return do_action(ctx, action::UseItem{item_index}); })};
          case SDLK_COMMA:
            if (event.key.mod & SDL_KMOD_SHIFT) return do_action(context, action::UseStairs(false));
            break;
          case SDLK_PERIOD:
            if (event.key.mod & SDL_KMOD_SHIFT) return do_action(context, action::UseStairs(true));
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
          return cmd_move(context, *dir);
        }
      } break;
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

 private:
  static auto cmd_move(GameContext& context, Position dir) -> StateReturnType {
    return do_action(context, action::Bump{dir});
  }

  static auto do_action(GameContext& context, action::Action& my_action) -> StateReturnType {
    auto& world = *context.world;
    return after_action(context, my_action.perform(context, world.active_player()));
  }

  static auto do_action(GameContext& context, action::Action&& my_action) -> StateReturnType {
    auto& world = *context.world;
    return after_action(context, my_action.perform(context, world.active_player()));
  }

  static auto after_action(GameContext& context, action::Result result) -> StateReturnType {
    if (std::holds_alternative<action::Failure>(result)) {
      context.world->log.append(std::get<action::Failure>(result).reason);
      return {};
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
