#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>

class Buffer;

enum class AttributeType : std::uint8_t {
  F32,
  I32,
  U32,
  I8,
  U8,
};

struct VertexAttribute final {
  std::int32_t size{};
  AttributeType type{AttributeType::F32};
  bool normalized{false};
  std::size_t offset{};
};

class VertexArray final {
public:
  VertexArray();
  ~VertexArray() noexcept;

  VertexArray(VertexArray const &) = delete;
  auto operator=(VertexArray const &) -> VertexArray & = delete;

  VertexArray(VertexArray &&other) noexcept;
  auto operator=(VertexArray &&other) noexcept -> VertexArray &;

  template <typename T>
  void setLayout(Buffer const &vertex_buffer,
                 std::initializer_list<VertexAttribute> attributes,
                 std::uint32_t binding_index = 0,
                 std::intptr_t buffer_offset = 0) const noexcept {
    setLayoutImpl(vertex_buffer, static_cast<std::int32_t>(sizeof(T)), attributes, binding_index, buffer_offset);
  }

  void setElementBuffer(Buffer const &buffer) const noexcept;

  void drawElements(std::uint32_t count) const noexcept;

private:
  void setLayoutImpl(Buffer const &vertex_buffer,
                     std::int32_t stride,
                     std::initializer_list<VertexAttribute> attributes,
                     std::uint32_t binding_index,
                     std::intptr_t buffer_offset) const noexcept;

  std::uint32_t id_{};
  mutable std::uint32_t next_attribute_index_{};
};
