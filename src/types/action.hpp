#pragma once
#include "action_result.hpp"
#include "actor_fwd.hpp"

struct GameContext;

namespace action {
class Action {
 public:
  virtual ~Action() = default;
  [[nodiscard]] virtual Result perform(GameContext& context, Actor& actor) = 0;
};
}  // namespace action
