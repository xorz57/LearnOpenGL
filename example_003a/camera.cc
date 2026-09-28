#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {

constexpr glm::vec3 kWorldUp{0.0F, 1.0F, 0.0F};
constexpr float kMovementSpeed{2.0F};
constexpr float kMouseSensitivity{0.1F};
constexpr float kMaxPitch{89.0F};
constexpr float kFov{45.0F};
constexpr float kNearPlane{0.1F};
constexpr float kFarPlane{100.0F};

}  // namespace

Camera::Camera(const glm::vec3& position) : position_{position} { UpdateCameraVectors(); }

auto Camera::ComputeViewMatrix() const -> glm::mat4 { return glm::lookAt(position_, position_ + front_, glm::normalize(glm::cross(right_, front_))); }

auto Camera::ComputeProjectionMatrix(float aspect_ratio) -> glm::mat4 { return glm::perspective(glm::radians(kFov), aspect_ratio, kNearPlane, kFarPlane); }

auto Camera::ProcessKeyboard(Movement direction, float delta_time, float speed_multiplier) -> void {
  const float velocity{kMovementSpeed * speed_multiplier * delta_time};
  switch (direction) {
    case Movement::kForward:
      position_ += front_ * velocity;
      break;
    case Movement::kBackward:
      position_ -= front_ * velocity;
      break;
    case Movement::kLeft:
      position_ -= right_ * velocity;
      break;
    case Movement::kRight:
      position_ += right_ * velocity;
      break;
    case Movement::kUp:
      position_ += kWorldUp * velocity;
      break;
    case Movement::kDown:
      position_ -= kWorldUp * velocity;
      break;
  }
}

auto Camera::ProcessMouseMovement(float x_offset, float y_offset, bool constrain_pitch) -> void {
  yaw_ += x_offset * kMouseSensitivity;
  pitch_ += y_offset * kMouseSensitivity;
  if (constrain_pitch) {
    pitch_ = glm::clamp(pitch_, -kMaxPitch, kMaxPitch);
  }
  UpdateCameraVectors();
}

auto Camera::UpdateCameraVectors() -> void {
  const glm::vec3 front{
      glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_)),
      glm::sin(glm::radians(pitch_)),
      glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_)),
  };
  front_ = glm::normalize(front);
  right_ = glm::normalize(glm::cross(front_, kWorldUp));
}
