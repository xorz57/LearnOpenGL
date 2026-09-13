#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,-warnings-as-errors)
Camera::Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch)
    : position_{position}, up_{world_up}, world_up_{world_up}, yaw_{yaw}, pitch_{glm::clamp(pitch, -89.0F, 89.0F)} {
  updateCameraVectors();
}

auto Camera::computeViewMatrix() const -> glm::mat4 { return glm::lookAt(position_, position_ + front_, up_); }

auto Camera::computeProjectionMatrix(float aspect_ratio) const -> glm::mat4 {
  return glm::perspective(glm::radians(fov_), aspect_ratio, near_plane_, far_plane_);
}

void Camera::processKeyboard(Movement direction, float delta_time, float speed_multiplier) {
  auto const velocity{movement_speed_ * speed_multiplier * delta_time};
  switch (direction) {
  case Movement::Forward:
    position_ += front_ * velocity;
    break;
  case Movement::Backward:
    position_ -= front_ * velocity;
    break;
  case Movement::Left:
    position_ -= right_ * velocity;
    break;
  case Movement::Right:
    position_ += right_ * velocity;
    break;
  case Movement::Up:
    position_ += world_up_ * velocity;
    break;
  case Movement::Down:
    position_ -= world_up_ * velocity;
    break;
  }
}

void Camera::processMouseMovement(float x_offset, float y_offset, bool constrain_pitch) {
  applyYawPitchOffset(x_offset, y_offset, constrain_pitch);
}

void Camera::processMousePan(float x_offset, float y_offset) noexcept {
  position_ += (-right_ * x_offset + up_ * y_offset) * pan_sensitivity_;
}

void Camera::orbit(glm::vec3 const &pivot, float x_offset, float y_offset) noexcept {
  constexpr float min_orbit_distance{0.01F};
  auto const distance{glm::max(glm::length(position_ - pivot), min_orbit_distance)};
  applyYawPitchOffset(x_offset, y_offset, true);
  position_ = pivot - front_ * distance;
}

void Camera::applyYawPitchOffset(float x_offset, float y_offset, bool constrain_pitch) noexcept {
  yaw_ += x_offset * mouse_sensitivity_;
  pitch_ += y_offset * mouse_sensitivity_;
  if (constrain_pitch) {
    pitch_ = glm::clamp(pitch_, -89.0F, 89.0F);
  }
  updateCameraVectors();
}

void Camera::updateCameraVectors() {
  glm::vec3 front{
      glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_)),
      glm::sin(glm::radians(pitch_)),
      glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_)),
  };
  front_ = glm::normalize(front);
  right_ = glm::normalize(glm::cross(front_, world_up_));
  up_ = glm::normalize(glm::cross(right_, front_));
}
