#include "vertex_array.h"

#include "buffer.h"

#include <glad/gl.h>

#include <utility>

VertexArray::VertexArray() { glCreateVertexArrays(1, &id_); }

VertexArray::~VertexArray() noexcept { glDeleteVertexArrays(1, &id_); }

VertexArray::VertexArray(VertexArray &&other) noexcept
    : id_(std::exchange(other.id_, 0)), next_attribute_index_(std::exchange(other.next_attribute_index_, 0)) {}

auto VertexArray::operator=(VertexArray &&other) noexcept -> VertexArray & {
  if (this != &other) {
    glDeleteVertexArrays(1, &id_);
    id_ = std::exchange(other.id_, 0);
    next_attribute_index_ = std::exchange(other.next_attribute_index_, 0);
  }
  return *this;
}

void VertexArray::setLayoutImpl(Buffer const &vertex_buffer,
                                std::int32_t stride,
                                std::initializer_list<VertexAttribute> attributes,
                                std::uint32_t binding_index,
                                std::intptr_t buffer_offset) const noexcept {
  glVertexArrayVertexBuffer(id_, binding_index, vertex_buffer.getId(), buffer_offset, stride);

  for (auto const &attribute : attributes) {
    std::uint32_t const attribute_index{next_attribute_index_++};
    glEnableVertexArrayAttrib(id_, attribute_index);
    auto const relative_offset{static_cast<std::uint32_t>(attribute.offset)};
    auto const normalized{attribute.normalized ? GL_TRUE : GL_FALSE};
    switch (attribute.type) {
    case AttributeType::F32:
      glVertexArrayAttribFormat(id_, attribute_index, attribute.size, GL_FLOAT, GL_FALSE, relative_offset);
      break;
    case AttributeType::I8:
      glVertexArrayAttribFormat(id_, attribute_index, attribute.size, GL_BYTE, normalized, relative_offset);
      break;
    case AttributeType::U8:
      glVertexArrayAttribFormat(id_, attribute_index, attribute.size, GL_UNSIGNED_BYTE, normalized, relative_offset);
      break;
    case AttributeType::I32:
      glVertexArrayAttribIFormat(id_, attribute_index, attribute.size, GL_INT, relative_offset);
      break;
    case AttributeType::U32:
      glVertexArrayAttribIFormat(id_, attribute_index, attribute.size, GL_UNSIGNED_INT, relative_offset);
      break;
    }
    glVertexArrayAttribBinding(id_, attribute_index, binding_index);
  }
}

void VertexArray::setElementBuffer(Buffer const &buffer) const noexcept {
  glVertexArrayElementBuffer(id_, buffer.getId());
}

void VertexArray::drawElements(std::uint32_t count) const noexcept {
  glBindVertexArray(id_);
  glDrawElements(GL_TRIANGLES, static_cast<std::int32_t>(count), GL_UNSIGNED_INT, static_cast<void *>(nullptr));
}
