#pragma once
#include <fmt/core.h>

#include "../globals.hpp"
#include "base.hpp"

namespace action {
class Pickup : public Action {
 public:
  [[nodiscard]] Result perform(GameContext& context, Actor& actor) override {
    auto& world = *context.world;
    Map& map = world.active_map();
    auto [first, last] = map.items.equal_range(actor.pos);
    if (first == map.items.end()) {
      return Failure{"Nothing to pickup!"};
    }
    auto& item = first->second;
    auto stack_item =
        std::find_if(actor.stats.inventory.begin(), actor.stats.inventory.end(), [&](auto& inventory_item) {
          return inventory_item->get_name() == item->get_name();
        });

    // Take or stack the item.
    world.log.append(fmt::format("You take the {}.", item->get_name()));
    if (stack_item != actor.stats.inventory.end()) {
      (*stack_item)->count += item->count;
    } else {
      actor.stats.inventory.emplace_back(std::move(item));
    }
    map.items.erase(first);
    return Success{};
  };
};

}  // namespace action
