#pragma once

#include <glm/glm.hpp>

struct Material final {
  alignas(16) glm::vec3 diffuse{};
  alignas(16) glm::vec3 specular{};
  float shininess{};
};
