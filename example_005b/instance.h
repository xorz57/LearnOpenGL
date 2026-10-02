#pragma once

#include <glm/glm.hpp>

#include <cstdint>

struct Instance final {
  glm::mat4 model{};
  glm::mat3 normal_matrix{};
  std::uint32_t material_index{};
};
