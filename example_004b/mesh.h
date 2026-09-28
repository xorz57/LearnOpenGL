#pragma once

#include "instance.h"
#include "vertex.h"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>

class Mesh final {
 public:
  enum class Error : std::uint8_t {
    kVerticesEmpty,
    kIndicesEmpty,
    kInstancesEmpty,
  };

  ~Mesh();

  Mesh(const Mesh&) = delete;
  auto operator=(const Mesh&) -> Mesh& = delete;

  Mesh(Mesh&& other) noexcept;
  auto operator=(Mesh&& other) noexcept -> Mesh&;

  [[nodiscard]] static auto Create(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices, std::span<const Instance> instances) -> std::expected<Mesh, Error>;

  auto Reset() -> void;

  auto Draw() const -> void;

 private:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  explicit Mesh(std::uint32_t vbo, std::uint32_t ebo, std::uint32_t ibo, std::uint32_t vao, std::size_t indices_size, std::size_t instances_size)
      : vbo_{vbo}, ebo_{ebo}, ibo_{ibo}, vao_{vao}, indices_size_{indices_size}, instances_size_{instances_size} {}

  std::uint32_t vbo_{};
  std::uint32_t ebo_{};
  std::uint32_t ibo_{};
  std::uint32_t vao_{};

  std::size_t indices_size_{};
  std::size_t instances_size_{};
};
