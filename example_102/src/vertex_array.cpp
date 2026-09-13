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

void VertexArray::setVertexBuffer(Buffer const &vertex_buffer,
                                  BufferLayout const &layout,
                                  std::uint32_t binding_index,
                                  std::intptr_t buffer_offset) const noexcept {
  glVertexArrayVertexBuffer(id_, binding_index, vertex_buffer.getId(), buffer_offset, layout.getStride());

  for (auto const &element : layout) {
    std::uint32_t const attribute_index{next_attribute_index_++};
    glEnableVertexArrayAttrib(id_, attribute_index);
    auto const relative_offset{static_cast<std::uint32_t>(element.offset)};
    auto const component_count{getShaderDataTypeComponentCount(element.type)};
    auto const normalized{element.normalized ? GL_TRUE : GL_FALSE};
    switch (element.type) {
    case ShaderDataType::Float:
    case ShaderDataType::Float2:
    case ShaderDataType::Float3:
    case ShaderDataType::Float4:
      glVertexArrayAttribFormat(id_, attribute_index, component_count, GL_FLOAT, GL_FALSE, relative_offset);
      break;
    case ShaderDataType::Int:
    case ShaderDataType::Int2:
    case ShaderDataType::Int3:
    case ShaderDataType::Int4:
      glVertexArrayAttribIFormat(id_, attribute_index, component_count, GL_INT, relative_offset);
      break;
    case ShaderDataType::UInt:
    case ShaderDataType::UInt2:
    case ShaderDataType::UInt3:
    case ShaderDataType::UInt4:
      glVertexArrayAttribIFormat(id_, attribute_index, component_count, GL_UNSIGNED_INT, relative_offset);
      break;
    case ShaderDataType::Byte4:
      glVertexArrayAttribFormat(id_, attribute_index, component_count, GL_BYTE, normalized, relative_offset);
      break;
    case ShaderDataType::UByte4:
      glVertexArrayAttribFormat(id_, attribute_index, component_count, GL_UNSIGNED_BYTE, normalized, relative_offset);
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
