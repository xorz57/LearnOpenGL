#include "mesh.h"

#include "buffer.h"
#include "instance.h"
#include "vertex.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <utility>

Mesh::~Mesh() { Reset(); }

Mesh::Mesh(Mesh &&other) noexcept
    : vertex_buffer_{std::move(other.vertex_buffer_)}, index_buffer_{std::move(other.index_buffer_)},
      instance_buffer_{std::move(other.instance_buffer_)}, vao_{std::exchange(other.vao_, 0)},
      indices_size_{std::exchange(other.indices_size_, 0)}, instances_size_{std::exchange(other.instances_size_, 0)} {}

auto Mesh::operator=(Mesh &&other) noexcept -> Mesh & {
  if (this != &other) {
    Reset();
    vertex_buffer_ = std::move(other.vertex_buffer_);
    index_buffer_ = std::move(other.index_buffer_);
    instance_buffer_ = std::move(other.instance_buffer_);
    vao_ = std::exchange(other.vao_, 0);
    indices_size_ = std::exchange(other.indices_size_, 0);
    instances_size_ = std::exchange(other.instances_size_, 0);
  }
  return *this;
}

auto Mesh::Create(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices,
                  std::span<const Instance> instances) -> std::expected<Mesh, Error> {
  if (vertices.empty()) {
    spdlog::error("Mesh has no vertices");
    return std::unexpected{Error::kVerticesEmpty};
  }
  if (indices.empty()) {
    spdlog::error("Mesh has no indices");
    return std::unexpected{Error::kIndicesEmpty};
  }
  if (instances.empty()) {
    spdlog::error("Mesh has no instances");
    return std::unexpected{Error::kInstancesEmpty};
  }

  auto vertex_buffer{Buffer::Create(std::as_bytes(vertices))};
  if (!vertex_buffer.has_value()) {
    return std::unexpected{Error::kBufferCreateFailed};
  }
  auto index_buffer{Buffer::Create(std::as_bytes(indices))};
  if (!index_buffer.has_value()) {
    return std::unexpected{Error::kBufferCreateFailed};
  }
  auto instance_buffer{Buffer::Create(std::as_bytes(instances))};
  if (!instance_buffer.has_value()) {
    return std::unexpected{Error::kBufferCreateFailed};
  }

  std::uint32_t vao{};
  ::glCreateVertexArrays(1, &vao);

  ::glVertexArrayVertexBuffer(vao, 0, vertex_buffer->GetHandle(), 0, static_cast<std::int32_t>(sizeof(Vertex)));
  ::glVertexArrayVertexBuffer(vao, 1, instance_buffer->GetHandle(), 0, static_cast<std::int32_t>(sizeof(Instance)));
  ::glVertexArrayElementBuffer(vao, index_buffer->GetHandle());

  ::glEnableVertexArrayAttrib(vao, 0);
  ::glEnableVertexArrayAttrib(vao, 1);
  ::glEnableVertexArrayAttrib(vao, 2);
  ::glEnableVertexArrayAttrib(vao, 3);
  ::glEnableVertexArrayAttrib(vao, 4);
  ::glEnableVertexArrayAttrib(vao, 5);
  ::glEnableVertexArrayAttrib(vao, 6);
  ::glEnableVertexArrayAttrib(vao, 7);

  ::glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  ::glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  ::glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
  ::glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (0 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 4, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (1 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 5, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (2 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 6, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (3 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 7, 3, GL_FLOAT, GL_FALSE, offsetof(Instance, color));

  ::glVertexArrayBindingDivisor(vao, 0, 0);
  ::glVertexArrayBindingDivisor(vao, 1, 1);

  ::glVertexArrayAttribBinding(vao, 0, 0);
  ::glVertexArrayAttribBinding(vao, 1, 0);
  ::glVertexArrayAttribBinding(vao, 2, 0);
  ::glVertexArrayAttribBinding(vao, 3, 1);
  ::glVertexArrayAttribBinding(vao, 4, 1);
  ::glVertexArrayAttribBinding(vao, 5, 1);
  ::glVertexArrayAttribBinding(vao, 6, 1);
  ::glVertexArrayAttribBinding(vao, 7, 1);

  return Mesh{std::move(*vertex_buffer), std::move(*index_buffer), std::move(*instance_buffer), vao, indices.size(),
              instances.size()};
}

auto Mesh::Reset() -> void {
  if (vao_ != 0) {
    ::glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
  }
  instance_buffer_.Reset();
  index_buffer_.Reset();
  vertex_buffer_.Reset();
  indices_size_ = 0;
  instances_size_ = 0;
}

auto Mesh::Draw() const -> void {
  if (vao_ == 0) {
    spdlog::error("Mesh not initialized");
    return;
  }

  ::glBindVertexArray(vao_);
  ::glDrawElementsInstanced(GL_TRIANGLES, static_cast<std::int32_t>(indices_size_), GL_UNSIGNED_INT,
                            static_cast<void *>(nullptr), static_cast<std::int32_t>(instances_size_));
}
