#pragma once
#include <fmt/core.h>

#include "../globals.hpp"
#include "../xp.hpp"
#include "menu.hpp"

namespace state {
class LevelUp : public Menu {
 public:
  explicit LevelUp(GameContext& context, int selected = 0)
      : Menu{
            MenuItems{
                {"Constitution (+20HP)",
                 [](GameContext& ctx) {
                   ctx.world->active_player().stats.hp += 20;
                   ctx.world->active_player().stats.max_hp += 20;
                   return level_up_done(ctx);
                 }},
                {"Strength (+1 attack)",
                 [](GameContext& ctx) {
                   ctx.world->active_player().stats.attack += 1;
                   return level_up_done(ctx);
                 }},
                {"Agility (+1 defense)",
                 [](GameContext& ctx) {
                   ctx.world->active_player().stats.defense += 1;
                   return level_up_done(ctx);
                 }}},
            selected} {
    assert(context.world);
    context.world->log.append(
        fmt::format(
            "Your battle skills grow stronger! You reached level {}.", context.world->active_player().stats.level + 1));
  }

 private:
  static auto level_up_done(GameContext& ctx) -> StateReturnType {
    auto& player = ctx.world->active_player();
    player.stats.xp -= next_level_xp(player.stats.level);
    ++player.stats.level;
    return Reset{};
  }
};
}  // namespace state
