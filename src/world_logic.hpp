#pragma once
// Phase 3 stub - will be expanded with full version in Phase 4

#include "types/actor.hpp"
#include "types/world.hpp"

/// Call function (Actor&) -> void on all active actors.
template <typename WithActorFunc>
inline auto with_active_actors(World& world, const WithActorFunc function) {
  for (const auto& actor_id : world.active_actors) function(world.get(actor_id));
}
template <typename WithActorFunc>
inline auto with_active_actors(const World& world, const WithActorFunc function) {
  for (const auto& actor_id : world.active_actors) function(world.get(actor_id));
}
