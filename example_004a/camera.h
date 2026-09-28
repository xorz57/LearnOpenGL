#pragma once

#include <glm/glm.hpp>

#include <cstdint>

class Camera final {
 public:
  enum class Movement : std::uint8_t {
    kForward,
    kBackward,
    kLeft,
    kRight,
    kUp,
    kDown,
  };

  explicit Camera(const glm::vec3& position = {0.0F, 0.0F, 0.0F});

  [[nodiscard]] auto ComputeViewMatrix() const -> glm::mat4;
  [[nodiscard]] static auto ComputeProjectionMatrix(float aspect_ratio) -> glm::mat4;

  auto ProcessKeyboard(Movement direction, float delta_time, float speed_multiplier = 1.0F) -> void;
  auto ProcessMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true) -> void;

  [[nodiscard]] auto GetPosition() const -> glm::vec3 { return position_; }

 private:
  auto UpdateCameraVectors() -> void;

  glm::vec3 position_;
  glm::vec3 front_{0.0F, 0.0F, -1.0F};
  glm::vec3 right_{1.0F, 0.0F, 0.0F};
  float yaw_{-90.0F};
  float pitch_{0.0F};
};
