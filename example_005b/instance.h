#pragma once

#include <cstdint>

struct Instance final {
  std::uint32_t transform_index{};
  std::uint32_t material_index{};
};
