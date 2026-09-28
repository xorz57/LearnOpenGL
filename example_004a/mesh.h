#pragma once

#include "vertex.h"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>

class Mesh final {
 public:
  enum class Error : std::uint8_t {
    kEmptyVertices,
    kEmptyIndices,
  };

  ~Mesh();

  Mesh(const Mesh&) = delete;
  auto operator=(const Mesh&) -> Mesh& = delete;

  Mesh(Mesh&& other) noexcept;
  auto operator=(Mesh&& other) noexcept -> Mesh&;

  [[nodiscard]] static auto Create(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices) -> std::expected<Mesh, Error>;

  auto Reset() noexcept -> void;

  auto Draw() const -> void;

 private:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  explicit Mesh(std::uint32_t vbo, std::uint32_t ebo, std::uint32_t vao, std::size_t indices_size) noexcept : vbo_{vbo}, ebo_{ebo}, vao_{vao}, indices_size_{indices_size} {}

  std::uint32_t vbo_{};
  std::uint32_t ebo_{};
  std::uint32_t vao_{};

  std::size_t indices_size_{};
};
