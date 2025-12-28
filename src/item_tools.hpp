#pragma once

#include <algorithm>
#include <cassert>

#include "types/actor.hpp"
#include "types/item.hpp"

/// Consume one count of `item` and if it is exhausted then delete it.  Return `item` if it still exists.
inline auto consume_discard_item(Actor& actor, Item* item) -> Item* {
  assert(item);
  assert(item->count > 0);

  auto& inventory = actor.stats.inventory;
  auto item_it = std::ranges::find_if(inventory, [&item](const auto& it) { return it.get() == item; });
  assert(item_it != inventory.end());
  --item->count;
  if (item->count <= 0) {
    inventory.erase(item_it);
    return nullptr;
  }
  return item;
}
