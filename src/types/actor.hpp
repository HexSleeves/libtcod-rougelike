#pragma once
#include <libtcod.hpp>
#include <memory>
#include <string>

#include "action.hpp"
#include "actor_id.hpp"
#include "position.hpp"
#include "stats.hpp"

struct Actor {
  int ch;
  std::string name;
  tcod::ColorRGB fg;
  std::unique_ptr<action::Action> ai;

  ActorID id;
  Stats stats;
  Position pos;
};
