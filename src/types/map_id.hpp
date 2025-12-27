#pragma once
#include <functional>
#include <string>

struct MapID {
  friend bool operator==(const MapID& lhs, const MapID& rhs) noexcept {
    return lhs.name == rhs.name && lhs.level == rhs.level;
  }
  friend bool operator!=(const MapID& lhs, const MapID& rhs) noexcept { return !(lhs == rhs); }

  std::string name;
  int level = 0;
};

template <>
struct std::hash<MapID> {
  std::size_t operator()(const MapID& id) const {
    return std::hash<decltype(id.name)>{}(id.name) ^ std::hash<decltype(id.level)>{}(id.level);
  }
};
