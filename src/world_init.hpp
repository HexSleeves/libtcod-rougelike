#pragma once
#include <memory>
#include <random>

#include "constants.hpp"
#include "procgen/caves.hpp"
#include "types/world.hpp"

/// Create a new world with procedurally generated dungeon
inline auto new_world() -> std::unique_ptr<World> {
  auto world = std::make_unique<World>();

  // Initialize RNG
  world->rng.seed(std::random_device{}());

  // Create player actor
  Actor player;
  player.id = ActorID{0};
  player.name = "Player";
  player.ch = '@';
  player.fg = tcod::ColorRGB{255, 255, 255};
  player.stats.max_hp = 30;
  player.stats.hp = 30;
  player.stats.attack = 5;
  player.stats.defense = 2;
  player.stats.level = 1;
  player.stats.xp = 0;

  world->actors[ActorID{0}] = std::move(player);
  world->active_actors.insert(ActorID{0});
  world->schedule.push_back(ActorID{0});

  // Generate first level using procedural generation
  procgen::generate_level(*world, 1);

  world->log.append("Welcome to the dungeon!", constants::TEXT_COLOR_DEFAULT);

  return world;
}
