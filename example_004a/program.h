#pragma once

#include "shader.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <expected>

class Program final {
 public:
  enum class Error : std::uint8_t {
    kLinkFailed,
  };

  ~Program();

  Program(const Program&) = delete;
  auto operator=(const Program&) -> Program& = delete;

  Program(Program&& other) noexcept;
  auto operator=(Program&& other) noexcept -> Program&;

  [[nodiscard]] static auto Create(const Shader& vertex_shader, const Shader& fragment_shader) -> std::expected<Program, Error>;

  auto Reset() noexcept -> void;

  [[nodiscard]] auto GetHandle() const noexcept -> std::uint32_t { return handle_; }

  auto Use() const -> void;

  auto SetUniform(const char* name, bool value) const -> void;

  auto SetUniform(const char* name, float v0) const -> void;
  auto SetUniform(const char* name, float v0, float v1) const -> void;
  auto SetUniform(const char* name, float v0, float v1, float v2) const -> void;
  auto SetUniform(const char* name, float v0, float v1, float v2, float v3) const -> void;

  auto SetUniform(const char* name, std::int32_t v0) const -> void;
  auto SetUniform(const char* name, std::int32_t v0, std::int32_t v1) const -> void;
  auto SetUniform(const char* name, std::int32_t v0, std::int32_t v1, std::int32_t v2) const -> void;
  auto SetUniform(const char* name, std::int32_t v0, std::int32_t v1, std::int32_t v2, std::int32_t v3) const -> void;

  auto SetUniform(const char* name, std::uint32_t v0) const -> void;
  auto SetUniform(const char* name, std::uint32_t v0, std::uint32_t v1) const -> void;
  auto SetUniform(const char* name, std::uint32_t v0, std::uint32_t v1, std::uint32_t v2) const -> void;
  auto SetUniform(const char* name, std::uint32_t v0, std::uint32_t v1, std::uint32_t v2, std::uint32_t v3) const -> void;

  auto SetUniform(const char* name, const glm::bvec2& value) const -> void;
  auto SetUniform(const char* name, const glm::bvec3& value) const -> void;
  auto SetUniform(const char* name, const glm::bvec4& value) const -> void;

  auto SetUniform(const char* name, const glm::vec2& value) const -> void;
  auto SetUniform(const char* name, const glm::vec3& value) const -> void;
  auto SetUniform(const char* name, const glm::vec4& value) const -> void;

  auto SetUniform(const char* name, const glm::ivec2& value) const -> void;
  auto SetUniform(const char* name, const glm::ivec3& value) const -> void;
  auto SetUniform(const char* name, const glm::ivec4& value) const -> void;

  auto SetUniform(const char* name, const glm::uvec2& value) const -> void;
  auto SetUniform(const char* name, const glm::uvec3& value) const -> void;
  auto SetUniform(const char* name, const glm::uvec4& value) const -> void;

  auto SetUniform(const char* name, const glm::mat2& value) const -> void;
  auto SetUniform(const char* name, const glm::mat3& value) const -> void;
  auto SetUniform(const char* name, const glm::mat4& value) const -> void;

 private:
  explicit Program(std::uint32_t handle) noexcept : handle_{handle} {}

  std::uint32_t handle_{};
};
