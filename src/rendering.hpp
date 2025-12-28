#pragma once

#include <SDL3/SDL.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include "constants.hpp"
#include "globals.hpp"
#include "world_logic.hpp"
#include "xp.hpp"

inline void render_map(tcod::Console& console, const Map& map, bool show_all = false) {
  const int x_min = 0;
  const int x_max = std::min(console.get_width(), map.get_width());
  const int y_min = 0;
  const int y_max = std::min(console.get_height(), map.get_height());

  for (int y{y_min}; y < y_max; ++y) {
    for (int x{x_min}; x < x_max; ++x) {
      // Bounds check removed due to pre-calculated loops.
      if (!show_all && !map.explored.at({x, y})) continue;
      console.at({x, y}) = map.tiles.at({x, y}) == Tiles::floor
                               ? TCOD_ConsoleTile{'.', tcod::ColorRGB{128, 128, 128}, tcod::ColorRGB{0, 0, 0}}
                               : TCOD_ConsoleTile{'#', tcod::ColorRGB{128, 128, 128}, tcod::ColorRGB{0, 0, 0}};
      if (!map.visible.at({x, y})) {
        console.at({x, y}).fg.r /= 2;
        console.at({x, y}).fg.g /= 2;
        console.at({x, y}).fg.b /= 2;
        console.at({x, y}).bg.r /= 2;
        console.at({x, y}).bg.g /= 2;
        console.at({x, y}).bg.b /= 2;
      }
    }
  }
}
inline void render_map(GameContext& context) {
  const auto& map = context.world->active_map();
  render_map(context.console, map);

  const auto can_draw = [&](Position map_pos) {
    if (!map.visible.in_bounds(map_pos)) return false;
    if (!map.visible.at(map_pos)) return false;
    if (!context.console.in_bounds(map_pos)) return false;
    return true;
  };

  for (const auto& [fixture_pos, fixture] : map.fixtures) {
    if (!can_draw(fixture_pos)) continue;
    context.console.at(fixture_pos).ch = fixture.ch;
    context.console.at(fixture_pos).fg = fixture.fg;
  }

  for (const auto& [item_pos, item] : map.items) {
    if (!can_draw(item_pos)) continue;
    const auto& [item_ch, item_fg] = item->get_graphic();
    context.console.at(item_pos).ch = item_ch;
    context.console.at(item_pos).fg = item_fg;
  }

  with_active_actors(*context.world, [&](const Actor& actor) {
    if (!can_draw(actor.pos)) return;
    context.console.at(actor.pos).ch = actor.ch;
    context.console.at(actor.pos).fg = actor.fg;
  });

  if (context.controller.cursor) {
    const auto& cursor = *context.controller.cursor;
    if (context.console.in_bounds(cursor) && map.visible.in_bounds(cursor)) {
      auto& cursor_tile = context.console.at(cursor);
      cursor_tile = {cursor_tile.ch, tcod::ColorRGB{0, 0, 0}, tcod::ColorRGB{255, 255, 255}};
    }
  }
}
inline void render_map() { /* Removed global overload */ }

inline void render_log(GameContext& context) {
  const int log_x = 22;
  const int log_width = context.console.get_width() - log_x;
  const int log_height = context.console.get_height() - constants::MAP_HEIGHT;

  // Optimization: Resize log_console only if necessary (initially 0x0)
  if (context.log_console.get_width() != log_width || context.log_console.get_height() != log_height) {
    context.log_console = tcod::Console{log_width, log_height};
  }

  context.log_console.clear();  // Important to clear reused console

  int y = context.log_console.get_height();
  for (auto it = context.world->log.messages.crbegin(); it != context.world->log.messages.crend(); ++it) {
    const auto& msg = *it;
    auto print_msg = [&](std::string_view text, const tcod::ColorRGB& fg) {
      y -= tcod::get_height_rect(context.log_console.get_width(), text);
      tcod::print_rect(context.log_console, {0, y, 0, context.log_console.get_width()}, text, fg, {});
    };
    if (msg.count > 1) {
      print_msg(fmt::format("{} (x{})", msg.text, msg.count), msg.fg);
    } else {
      print_msg(msg.text, msg.fg);
    }
    if (y < 0) break;
  }
  tcod::blit(context.console, context.log_console, {log_x, 45});
}

inline void draw_bar(
    tcod::Console& console,
    int x,
    int y,
    int width,
    float fill,
    const tcod::ColorRGB& fill_color,
    const tcod::ColorRGB& bg_color,
    std::string_view label) {
  const int fill_width = static_cast<int>(fill * width);
  tcod::draw_rect(console, {x, y, width, 1}, 0, std::nullopt, bg_color);
  if (fill_width > 0) {
    tcod::draw_rect(console, {x, y, fill_width, 1}, 0, std::nullopt, fill_color);
  }
  tcod::print(console, {x + width / 2, y}, label, tcod::ColorRGB{255, 255, 255}, std::nullopt, TCOD_CENTER);
}

inline void render_mouse_look(GameContext& context) {
  if (!context.controller.cursor) return;

  const auto& map = context.world->active_map();
  if (!(map.visible.in_bounds(*context.controller.cursor) && map.visible.at(*context.controller.cursor))) return;

  auto cursor_desc = std::vector<std::string>{};
  if (const auto found_fixture = map.fixtures.find(*context.controller.cursor); found_fixture != map.fixtures.end()) {
    cursor_desc.emplace_back(found_fixture->second.name);
  }

  with_active_actors(*context.world, [&](const Actor& actor) {
    if (actor.pos == *context.controller.cursor) {
      cursor_desc.emplace_back(actor.name);
    }
  });

  if (!cursor_desc.empty()) {
    tcod::print(
        context.console,
        *context.controller.cursor,
        fmt::format("{}", fmt::join(cursor_desc, ", ")),
        tcod::ColorRGB{255, 255, 255},
        tcod::ColorRGB{0, 0, 0});
  }
}

inline void render_gui(GameContext& context) {
  const auto& player = context.world->active_player();
  const int hp_x = 1;
  const int hp_y = constants::MAP_HEIGHT + 1;

  draw_bar(
      context.console,
      hp_x,
      hp_y,
      20,
      static_cast<float>(player.stats.hp) / player.stats.max_hp,
      constants::HP_BAR_FILL,
      constants::HP_BAR_BACK,
      fmt::format(" HP: {}/{}", player.stats.hp, player.stats.max_hp));
  draw_bar(
      context.console,
      hp_x,
      hp_y + 1,
      20,
      static_cast<float>(player.stats.xp) / next_level_xp(player.stats.level),
      constants::XP_BAR_FILL,
      constants::XP_BAR_BACK,
      fmt::format(" XP: {}", player.stats.xp));
  render_log(context);
  render_mouse_look(context);
}

inline void render_all(GameContext& context) {
  render_map(context);
  render_gui(context);
}

// inline void main_redraw() { ... } // Removed
// inline auto debug_show_map(...) // Needs update if used, or removed

inline auto debug_show_map([[maybe_unused]] GameContext& context, [[maybe_unused]] const Map& map) -> void {
#ifndef NDEBUG
  render_map(context.console, map, true);
  context.context.present(context.console);
  SDL_Delay(100);
#endif
}
