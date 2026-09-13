#pragma once

#include <glm/glm.hpp>

#include <cstdint>

class Camera final {
public:
  enum class Movement : std::uint8_t {
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down,
  };

  explicit Camera(glm::vec3 position = {0.0F, 0.0F, 5.0F},
                  glm::vec3 world_up = {0.0F, 1.0F, 0.0F},
                  float yaw = -90.0F,
                  float pitch = 0.0F);

  [[nodiscard]] auto computeViewMatrix() const -> glm::mat4;
  [[nodiscard]] auto computeProjectionMatrix(float aspect_ratio) const -> glm::mat4;

  void processKeyboard(Movement direction, float delta_time, float speed_multiplier = 1.0F);
  void processMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true);
  void processMousePan(float x_offset, float y_offset) noexcept;
  void orbit(glm::vec3 const &pivot, float x_offset, float y_offset) noexcept;

private:
  void updateCameraVectors();
  void applyYawPitchOffset(float x_offset, float y_offset, bool constrain_pitch) noexcept;

  glm::vec3 position_;
  glm::vec3 front_{0.0F, 0.0F, -1.0F};
  glm::vec3 up_;
  glm::vec3 right_{1.0F, 0.0F, 0.0F};
  glm::vec3 world_up_;
  float yaw_;
  float pitch_;
  float movement_speed_{2.5F};
  float mouse_sensitivity_{0.1F};
  float pan_sensitivity_{0.02F};
  float fov_{45.0F};
  float near_plane_{0.1F};
  float far_plane_{100.0F};
};
