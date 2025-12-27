#pragma once
#include <fmt/core.h>

#include <cassert>
#include <gsl/gsl>

#include "../combat.hpp"
#include "base.hpp"

namespace action {
class UseItem : public Action {
 public:
  UseItem() = default;
  explicit UseItem(int item_index) : item_index_{item_index} { assert(item_index_ >= 0); }
  [[nodiscard]] Result perform(World& world, Actor& actor) override {
    if (item_index_ >= gsl::narrow<decltype(item_index_)>(actor.stats.inventory.size())) {
      return Failure{"You don't have that item."};
    }
    const auto& item = actor.stats.inventory.at(item_index_);
    return item->use_item(world, actor);
  };

 private:
  int item_index_ = 0;
};

}  // namespace action
