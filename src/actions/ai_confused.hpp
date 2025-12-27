#pragma once
#include "../globals.hpp"
#include "bump.hpp"

namespace action {
class ConfusedAI : public Action {
 public:
  [[nodiscard]] Result perform(GameContext& context, Actor& actor) override {
    auto& world = *context.world;
    const auto random_dir = Position{static_cast<int>(world.rng() % 3) - 1, static_cast<int>(world.rng() % 3) - 1};
    auto result = Bump(random_dir).perform(context, actor);
    if (std::holds_alternative<Failure>(result)) result = Success{};
    return result;
  };
};
}  // namespace action
