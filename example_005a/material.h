#pragma once

#include <glm/glm.hpp>

struct Material final {
  glm::vec3 diffuse{};
  glm::vec3 specular{};
  float shininess{};
};
