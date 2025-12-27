#pragma once
#include <fmt/core.h>

#include "types/actor.hpp"
#include "types/world.hpp"

namespace combat {
inline auto destroy(World& world, const Actor& target) {
  world.active_actors.erase(target.id);
  world.actors.erase(target.id);
}

inline auto kill(World& world, Actor& target) {
  if (target.id == ActorID{0}) {
    world.log.append(fmt::format("You have died!"));
  } else {
    world.active_player().stats.xp += target.stats.xp;
    if (world.active_map().visible.at(target.pos)) {
      world.log.append(fmt::format("The {} dies! You gain {} xp.", target.name, target.stats.xp));
    }
    destroy(world, target);
  }
}

inline auto apply_damage(World& world, Actor& target, int damage) {
  target.stats.hp -= damage;
  if (target.stats.hp <= 0) {
    kill(world, target);
  }
}

inline auto calculate_damage(World&, const Actor& target, int damage) -> int {
  damage -= target.stats.defense;
  damage = std::max(0, damage);
  return damage;
}

inline auto attack(World& world, Actor& self, Actor& target) {
  const auto damage = calculate_damage(world, target, self.stats.attack);
  if (const auto& map = world.active_map(); map.visible.at(self.pos) || map.visible.at(target.pos)) {
    if (damage > 0) {
      world.log.append(fmt::format("{} attacks {} for {} hit points.", self.name, target.name, damage));
    } else {
      world.log.append(fmt::format("{} attacks {} but it has no effect!", self.name, target.name));
    }
  }
  apply_damage(world, target, damage);
}

inline auto heal(World& world, Actor& target, int amount) {
  amount = std::min(amount, target.stats.max_hp - target.stats.hp);
  target.stats.hp += amount;
  if (const auto& map = world.active_map(); map.visible.at(target.pos)) {
    world.log.append(fmt::format("{} heals {} HP.", target.name, amount));
  }
  return amount;
}

}  // namespace combat
