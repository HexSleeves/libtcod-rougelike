#pragma once

#include <SDL3/SDL_events.h>

#include "state_result.hpp"

struct GameContext;  // Forward declaration

namespace state {
class State {
 public:
  using StateReturnType = Result;
  virtual ~State() = default;
  [[nodiscard]] virtual auto on_event(GameContext& context, SDL_Event& event) -> StateReturnType = 0;
  virtual auto on_draw(GameContext& context) -> void = 0;
};
}  // namespace state
