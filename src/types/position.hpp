#pragma once
#include <array>

struct Position {
  // Allow this struct to be used in subscript operators as {x, y}.
  operator std::array<int, 2>() const noexcept { return {x, y}; }
  friend Position operator+(const Position lhs, const Position rhs) noexcept { return {lhs.x + rhs.x, lhs.y + rhs.y}; }
  friend Position operator-(const Position lhs, const Position rhs) noexcept { return {lhs.x - rhs.x, lhs.y - rhs.y}; }
  friend bool operator==(const Position lhs, const Position rhs) noexcept { return lhs.x == rhs.x && lhs.y == rhs.y; }
  friend bool operator!=(const Position lhs, const Position rhs) noexcept { return !(lhs == rhs); }

  int x;
  int y;
};
template <>
struct std::hash<Position> {
  std::size_t operator()(const Position& pos) const { return pos.x + pos.y * 97; }
};
