#pragma once

#include "buffer.h"
#include "vertex.h"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <utility>

class Mesh final {
public:
  enum class Error : std::uint8_t {
    kVerticesEmpty,
    kIndicesEmpty,
    kBufferCreateFailed,
  };

  ~Mesh();

  Mesh(const Mesh &) = delete;
  auto operator=(const Mesh &) -> Mesh & = delete;

  Mesh(Mesh &&other) noexcept;
  auto operator=(Mesh &&other) noexcept -> Mesh &;

  [[nodiscard]] static auto Create(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices)
      -> std::expected<Mesh, Error>;

  auto Reset() -> void;

  auto Draw() const -> void;

private:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  explicit Mesh(Buffer vertex_buffer, Buffer index_buffer, std::uint32_t vao, std::size_t indices_size)
      : vertex_buffer_{std::move(vertex_buffer)}, index_buffer_{std::move(index_buffer)}, vao_{vao},
        indices_size_{indices_size} {}

  Buffer vertex_buffer_;
  Buffer index_buffer_;
  std::uint32_t vao_{};

  std::size_t indices_size_{};
};
