#pragma once
#include <memory>
#include <random>

#include "constants.hpp"
#include "maptools.hpp"
#include "types/world.hpp"

/// Create a simple test world for Phase 3
inline auto new_world() -> std::unique_ptr<World> {
  auto world = std::make_unique<World>();

  // Initialize RNG
  world->rng.seed(std::random_device{}());

  // Create a simple map
  MapID map_id{"dungeon", 1};
  Map map{constants::MAP_WIDTH, constants::MAP_HEIGHT};
  map.id = map_id;

  // Fill with floor tiles
  with_indexes(map.tiles, [&map](int x, int y) { map.tiles.at({x, y}) = Tiles::floor; });

  // Add walls around the border
  with_border(map.tiles, [&map](int x, int y) { map.tiles.at({x, y}) = Tiles::wall; });

  // Make everything explored and visible for testing
  with_indexes(map.explored, [&map](int x, int y) { map.explored.at({x, y}) = true; });
  with_indexes(map.visible, [&map](int x, int y) { map.visible.at({x, y}) = true; });

  // Add test fixture (stairs down)
  map.fixtures[{40, 23}] = Fixture{"Stairs Down", '>', tcod::ColorRGB{255, 255, 255}};

  world->maps[map_id] = std::move(map);
  world->current_map_id = map_id;

  // Create player actor
  Actor player;
  player.id = ActorID{0};
  player.pos = {40, 25};  // Center of map
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

  world->log.append("Welcome to the dungeon!", constants::TEXT_COLOR_DEFAULT);

  return world;
}
