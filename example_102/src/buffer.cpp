#include "buffer.h"

#include <glad/gl.h>

#include <utility>

Buffer::Buffer(void const *data, std::ptrdiff_t size) {
  glCreateBuffers(1, &id_);
  glNamedBufferStorage(id_, size, data, 0);
}

Buffer::~Buffer() noexcept { glDeleteBuffers(1, &id_); }

Buffer::Buffer(Buffer &&other) noexcept : id_(std::exchange(other.id_, 0)) {}

auto Buffer::operator=(Buffer &&other) noexcept -> Buffer & {
  if (this != &other) {
    glDeleteBuffers(1, &id_);
    id_ = std::exchange(other.id_, 0);
  }
  return *this;
}

auto Buffer::getId() const noexcept -> std::uint32_t { return id_; }
