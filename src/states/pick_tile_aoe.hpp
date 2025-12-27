#pragma once
#include "../distance.hpp"
#include "../globals.hpp"
#include "../maptools.hpp"
#include "../rendering.hpp"
#include "pick_tile.hpp"

namespace state {
class PickTileAreaOfEffect : public PickTile {
 public:
  using AOEFunction = std::function<bool(Position)>;
  PickTileAreaOfEffect(std::unique_ptr<State> parent, const PickFunction& on_pick, int radius_squared)
      : PickTile{std::move(parent), on_pick}, radius_squared_{radius_squared} {};

  auto on_draw(GameContext& context) -> void override {
    render_all(context);
    if (!context.controller.cursor) return;
    const auto& map = context.world->active_map();
    with_indexes(map, [this, &context, pos = *context.controller.cursor](int x, int y) {
      if (euclidean_squared(Position{x, y} - pos) >= radius_squared_) return;
      if (!context.console.in_bounds({x, y})) return;
      auto& tile = context.console.at({x, y});
      tile = {tile.ch, tcod::ColorRGB{0, 0, 0}, tcod::ColorRGB{255, 255, 255}};
    });
  }

 private:
  int radius_squared_ = 0;
};
}  // namespace state
