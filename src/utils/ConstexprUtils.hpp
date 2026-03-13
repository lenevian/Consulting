#pragma once

#include <cstdint>
#include <string_view>

namespace consulting {

constexpr uint32_t hash_string(std::string_view str) noexcept {
  uint32_t hash = 0x811c9dc5;
  for (char c : str) {
    hash ^= static_cast<uint32_t>(c);
    hash *= 0x01000193;
  }
  return hash;
}

}
