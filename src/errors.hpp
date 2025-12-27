#pragma once
#include <stdexcept>

// Custom exceptions for the game
class DataDirectoryNotFoundError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};
