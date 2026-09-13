#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>

class Buffer;

enum class ShaderDataType : std::uint8_t {
  Float,
  Float2,
  Float3,
  Float4,
  Int,
  Int2,
  Int3,
  Int4,
  UInt,
  UInt2,
  UInt3,
  UInt4,
  Byte4,
  UByte4,
};

[[nodiscard]] constexpr auto getShaderDataTypeSize(ShaderDataType type) noexcept -> std::size_t {
  switch (type) {
  case ShaderDataType::Float:
    return sizeof(float);
  case ShaderDataType::Float2:
    return sizeof(float) * 2;
  case ShaderDataType::Float3:
    return sizeof(float) * 3;
  case ShaderDataType::Float4:
    return sizeof(float) * 4;
  case ShaderDataType::Int:
    return sizeof(std::int32_t);
  case ShaderDataType::Int2:
    return sizeof(std::int32_t) * 2;
  case ShaderDataType::Int3:
    return sizeof(std::int32_t) * 3;
  case ShaderDataType::Int4:
    return sizeof(std::int32_t) * 4;
  case ShaderDataType::UInt:
    return sizeof(std::uint32_t);
  case ShaderDataType::UInt2:
    return sizeof(std::uint32_t) * 2;
  case ShaderDataType::UInt3:
    return sizeof(std::uint32_t) * 3;
  case ShaderDataType::UInt4:
    return sizeof(std::uint32_t) * 4;
  case ShaderDataType::Byte4:
    return sizeof(std::int8_t) * 4;
  case ShaderDataType::UByte4:
    return sizeof(std::uint8_t) * 4;
  }
  return 0;
}

[[nodiscard]] constexpr auto getShaderDataTypeComponentCount(ShaderDataType type) noexcept -> std::int32_t {
  switch (type) {
  case ShaderDataType::Float:
  case ShaderDataType::Int:
  case ShaderDataType::UInt:
    return 1;
  case ShaderDataType::Float2:
  case ShaderDataType::Int2:
  case ShaderDataType::UInt2:
    return 2;
  case ShaderDataType::Float3:
  case ShaderDataType::Int3:
  case ShaderDataType::UInt3:
    return 3;
  case ShaderDataType::Float4:
  case ShaderDataType::Int4:
  case ShaderDataType::UInt4:
  case ShaderDataType::Byte4:
  case ShaderDataType::UByte4:
    return 4;
  }
  return 0;
}

struct BufferElement final {
  ShaderDataType type;
  std::size_t size;
  std::size_t offset{};
  bool normalized{false};

  BufferElement(ShaderDataType element_type, bool is_normalized = false) noexcept
      : type(element_type), size(getShaderDataTypeSize(element_type)), normalized(is_normalized) {}
};

class BufferLayout final {
public:
  BufferLayout(std::initializer_list<BufferElement> elements) : elements_(elements) {
    std::size_t offset{0};
    for (auto &element : elements_) {
      element.offset = offset;
      offset += element.size;
    }
    stride_ = static_cast<std::int32_t>(offset);
  }

  [[nodiscard]] auto getStride() const noexcept -> std::int32_t { return stride_; }
  [[nodiscard]] auto getElements() const noexcept -> std::vector<BufferElement> const & { return elements_; }

  [[nodiscard]] auto begin() const noexcept { return elements_.begin(); }
  [[nodiscard]] auto end() const noexcept { return elements_.end(); }

private:
  std::vector<BufferElement> elements_;
  std::int32_t stride_{};
};

class VertexArray final {
public:
  VertexArray();
  ~VertexArray() noexcept;

  VertexArray(VertexArray const &) = delete;
  auto operator=(VertexArray const &) -> VertexArray & = delete;

  VertexArray(VertexArray &&other) noexcept;
  auto operator=(VertexArray &&other) noexcept -> VertexArray &;

  void setVertexBuffer(Buffer const &vertex_buffer,
                       BufferLayout const &layout,
                       std::uint32_t binding_index = 0,
                       std::intptr_t buffer_offset = 0) const noexcept;

  void setElementBuffer(Buffer const &buffer) const noexcept;

  void drawElements(std::uint32_t count) const noexcept;

private:
  std::uint32_t id_{};
  mutable std::uint32_t next_attribute_index_{};
};
