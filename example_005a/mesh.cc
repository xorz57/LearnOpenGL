#include "mesh.h"

#include "buffer.h"
#include "vertex.h"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <utility>

Mesh::~Mesh() { Reset(); }

Mesh::Mesh(Mesh &&other) noexcept
    : vertex_buffer_{std::move(other.vertex_buffer_)}, index_buffer_{std::move(other.index_buffer_)},
      vao_{std::exchange(other.vao_, 0)}, indices_size_{std::exchange(other.indices_size_, 0)} {}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh & {
  if (this != &other) {
    Reset();
    vertex_buffer_ = std::move(other.vertex_buffer_);
    index_buffer_ = std::move(other.index_buffer_);
    vao_ = std::exchange(other.vao_, 0);
    indices_size_ = std::exchange(other.indices_size_, 0);
  }
  return *this;
}

auto Mesh::Create(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices)
    -> std::expected<Mesh, Error> {
  if (vertices.empty()) {
    spdlog::error("Mesh has no vertices");
    return std::unexpected{Error::kVerticesEmpty};
  }
  if (indices.empty()) {
    spdlog::error("Mesh has no indices");
    return std::unexpected{Error::kIndicesEmpty};
  }

  auto vertex_buffer{Buffer::Create(std::as_bytes(vertices))};
  if (!vertex_buffer.has_value()) {
    return std::unexpected{Error::kBufferCreateFailed};
  }
  auto index_buffer{Buffer::Create(std::as_bytes(indices))};
  if (!index_buffer.has_value()) {
    return std::unexpected{Error::kBufferCreateFailed};
  }

  std::uint32_t vao{};
  ::glCreateVertexArrays(1, &vao);

  ::glVertexArrayVertexBuffer(vao, 0, vertex_buffer->GetHandle(), 0, static_cast<std::int32_t>(sizeof(Vertex)));
  ::glVertexArrayElementBuffer(vao, index_buffer->GetHandle());

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

  return Mesh{std::move(*vertex_buffer), std::move(*index_buffer), vao, indices.size()};
}

auto Mesh::Reset() -> void {
  if (vao_ != 0) {
    ::glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
  }
  index_buffer_.Reset();
  vertex_buffer_.Reset();
  indices_size_ = 0;
}

auto Mesh::Draw() const -> void {
  if (vao_ == 0) {
    spdlog::error("Mesh not initialized");
    return;
  }

  ::glBindVertexArray(vao_);
  ::glDrawElements(GL_TRIANGLES, static_cast<std::int32_t>(indices_size_), GL_UNSIGNED_INT,
                   static_cast<void *>(nullptr));
}
