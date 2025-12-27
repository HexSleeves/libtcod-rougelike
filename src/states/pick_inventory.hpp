#pragma once
#include <fmt/format.h>

#include <cassert>
#include <functional>
#include <libtcod.hpp>

#include "../globals.hpp"
#include "../types/state.hpp"

namespace state {
class PickInventory : public State {
 public:
  using PickFunction = std::function<StateReturnType(GameContext&, int)>;
  PickInventory(std::unique_ptr<State> parent, const PickFunction& on_pick)
      : parent_{std::move(parent)}, on_pick_{on_pick} {};

  auto on_event(GameContext& context, SDL_Event& event) -> StateReturnType override {
    assert(context.world);
    switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
          case SDLK_ESCAPE:
            return Change{std::move(parent_)};
          default:
            auto sym = event.key.key;
            if (SDLK_A <= sym && sym <= SDLK_Z) {
              return on_pick_(context, sym - SDLK_A);
            }
            break;
        }
        break;
      case SDL_EVENT_QUIT:
        return Quit{};
      default:
        break;
    }
    return {};
  }
  auto on_draw(GameContext& context) -> void override {
    parent_->on_draw(context);
    static constexpr auto INVENTORY_WIDTH = 50;
    static constexpr auto INVENTORY_HEIGHT = 28;
    static constexpr auto DECORATION =
        std::array<int, 9>{0x250c, 0x2500, 0x2510, 0x2502, 0, 0x2502, 0x2514, 0x2500, 0x2518};

    auto console = tcod::Console{INVENTORY_WIDTH, INVENTORY_HEIGHT};

    tcod::draw_frame(console, {0, 0, console.get_width(), console.get_height()}, DECORATION, {}, {});
    tcod::print(console, {2, 0}, "┤Inventory├", {}, {});
    tcod::print(
        console,
        {2, console.get_height() - 1},
        "[a-z]Use Item, [ESC]Cancel",
        tcod::ColorRGB{0, 0, 0},
        tcod::ColorRGB{255, 255, 255});
    if (context.world->active_player().stats.inventory.size()) {
      int shortcut = 'a';
      int y = 1;
      for (const auto& item : context.world->active_player().stats.inventory) {
        tcod::print(
            console, {1, y++}, fmt::format("({:c}) {} ({})", shortcut++, item->get_name(), item->count), {}, {});
      }
    } else {
      tcod::print(console, {1, 1}, "You have no items.", {}, {});
    }
    tcod::blit(
        context.console,
        console,
        {context.console.get_width() / 2 - console.get_width() / 2,
         context.console.get_height() / 2 - console.get_height() / 2});
  }

 private:
  std::unique_ptr<State> parent_;
  PickFunction on_pick_;
};
}  // namespace state
