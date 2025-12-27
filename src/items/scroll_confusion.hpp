#pragma once

#include <fmt/core.h>

#include "../globals.hpp"
#include "../item_tools.hpp"
#include "../states/pick_tile.hpp"
#include "../types/actor.hpp"
#include "../types/item.hpp"
#include "../types/world.hpp"
#include "../world_logic.hpp"

struct ConfusionScroll : public Item {
  int confuse_turns = 10;

  [[nodiscard]] std::string get_name() const override { return "scroll of confusion"; }
  [[nodiscard]] std::tuple<int, tcod::ColorRGB> get_graphic() const override {
    return {'#', tcod::ColorRGB{207, 63, 255}};
  }

  [[nodiscard]] action::Result use_item(GameContext& context, Actor& actor) override {
    auto on_pick = [&actor, this](GameContext& ctx, Position target_pos) -> state::Result {
      auto& world = *ctx.world;
      if (const auto& map = world.active_map(); !map.visible.in_bounds(target_pos) || !map.visible.at(target_pos)) {
        world.log.append("You can't see anything there!");
        return state::Reset{};
      }

      bool hit_target = false;
      with_actors_at(world, target_pos, [&world, &actor, this, &hit_target](Actor& target) {
        if (&target == &actor) return;
        hit_target = true;
        world.log.append(fmt::format("The eyes of the {} look vacant,\nas he starts to stumble around!", target.name));
        target.stats.confused_turns = std::max(target.stats.confused_turns, confuse_turns);
      });

      if (!hit_target) return state::Reset{};
      consume_discard_item(actor, this);
      return state::EndTurn{};
    };

    auto& world = *context.world;
    const auto& map = world.active_map();
    const auto is_not_self_and_visible = [&actor, &map](const Actor& other) {
      return other.id != actor.id && map.visible.at(other.pos);
    };

    const auto* nearest_visible_enemy = get_nearest_actor(world, actor.pos, is_not_self_and_visible);
    context.controller.cursor = nearest_visible_enemy ? nearest_visible_enemy->pos : actor.pos;
    return action::Poll{std::make_unique<state::PickTile>(std::move(context.state), on_pick)};
  };
};
