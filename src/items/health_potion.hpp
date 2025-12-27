#pragma once

#include <fmt/core.h>

#include "../combat.hpp"
#include "../globals.hpp"
#include "../item_tools.hpp"
#include "../types/actor.hpp"
#include "../types/item.hpp"
#include "../types/world.hpp"

struct HealthPotion : public Item {
  [[nodiscard]] std::string get_name() const override { return "health potion"; }
  [[nodiscard]] std::tuple<int, tcod::ColorRGB> get_graphic() const override { return {'!', {128, 21, 21}}; }
  [[nodiscard]] action::Result use_item(GameContext& context, Actor& actor) override {
    auto& world = *context.world;
    world.log.append(fmt::format("You drink the {}.", get_name()));
    combat::heal(world, actor, 4);
    consume_discard_item(actor, this);
    return action::Success{};
  };
};
