#include "mesh.h"

#include "vertex.h"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <utility>

Mesh::~Mesh() { Reset(); }

Mesh::Mesh(Mesh&& other) noexcept : vbo_{std::exchange(other.vbo_, 0)}, ebo_{std::exchange(other.ebo_, 0)}, vao_{std::exchange(other.vao_, 0)}, indices_size_{std::exchange(other.indices_size_, 0)} {}

auto Mesh::operator=(Mesh&& other) noexcept -> Mesh& {
  if (this != &other) {
    Reset();
    vbo_ = std::exchange(other.vbo_, 0);
    ebo_ = std::exchange(other.ebo_, 0);
    vao_ = std::exchange(other.vao_, 0);
    indices_size_ = std::exchange(other.indices_size_, 0);
  }
  return *this;
}

auto Mesh::Create(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices) -> std::expected<Mesh, Error> {
  if (vertices.empty()) {
    spdlog::error("Mesh has no vertices");
    return std::unexpected{Error::kVerticesEmpty};
  }
  if (indices.empty()) {
    spdlog::error("Mesh has no indices");
    return std::unexpected{Error::kIndicesEmpty};
  }

  std::uint32_t vbo{};
  std::uint32_t ebo{};
  std::uint32_t vao{};

  ::glCreateBuffers(1, &vbo);
  ::glCreateBuffers(1, &ebo);

  ::glCreateVertexArrays(1, &vao);

  ::glNamedBufferStorage(vbo, static_cast<std::ptrdiff_t>(vertices.size() * sizeof(Vertex)), vertices.data(), 0);
  ::glNamedBufferStorage(ebo, static_cast<std::ptrdiff_t>(indices.size() * sizeof(std::uint32_t)), indices.data(), 0);

  ::glVertexArrayVertexBuffer(vao, 0, vbo, 0, static_cast<std::int32_t>(sizeof(Vertex)));
  ::glVertexArrayElementBuffer(vao, ebo);

  ::glEnableVertexArrayAttrib(vao, 0);
  ::glEnableVertexArrayAttrib(vao, 1);
  ::glEnableVertexArrayAttrib(vao, 2);

  ::glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  ::glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  ::glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

  ::glVertexArrayBindingDivisor(vao, 0, 0);

  ::glVertexArrayAttribBinding(vao, 0, 0);
  ::glVertexArrayAttribBinding(vao, 1, 0);
  ::glVertexArrayAttribBinding(vao, 2, 0);

  return Mesh{vbo, ebo, vao, indices.size()};
}

auto Mesh::Reset() -> void {
  if (vao_ != 0) {
    ::glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
  }
  if (ebo_ != 0) {
    ::glDeleteBuffers(1, &ebo_);
    ebo_ = 0;
  }
  if (vbo_ != 0) {
    ::glDeleteBuffers(1, &vbo_);
    vbo_ = 0;
  }
  indices_size_ = 0;
}

auto Mesh::Draw() const -> void {
  if (vao_ == 0) {
    spdlog::error("Mesh not initialized");
    return;
  }

  ::glBindVertexArray(vao_);
  ::glDrawElements(GL_TRIANGLES, static_cast<std::int32_t>(indices_size_), GL_UNSIGNED_INT, static_cast<void*>(nullptr));
}
