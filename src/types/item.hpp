#pragma once
#include <libtcod/color.hpp>
#include <string>
#include <tuple>

// Minimal stub for Phase 3 - full implementation will be ported in Phase 5
struct Item {
  virtual ~Item() = default;
  [[nodiscard]] virtual std::tuple<int, tcod::ColorRGB> get_graphic() const { return {ch, fg}; }
  std::string name = "<undefined>";
  int ch = '?';
  tcod::ColorRGB fg = {255, 255, 255};
  int count = 1;
};
