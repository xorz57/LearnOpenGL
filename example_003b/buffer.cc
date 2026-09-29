#include "buffer.h"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <utility>

Buffer::~Buffer() { Reset(); }

Buffer::Buffer(Buffer &&other) noexcept
    : handle_{std::exchange(other.handle_, 0)}, size_{std::exchange(other.size_, 0)},
      usage_{std::exchange(other.usage_, Usage::kStatic)} {}

auto Buffer::operator=(Buffer &&other) noexcept -> Buffer & {
  if (this != &other) {
    Reset();
    handle_ = std::exchange(other.handle_, 0);
    size_ = std::exchange(other.size_, 0);
    usage_ = std::exchange(other.usage_, Usage::kStatic);
  }
  return *this;
}

auto Buffer::Create(std::span<const std::byte> data, Usage usage) -> std::expected<Buffer, Error> {
  if (data.empty()) {
    spdlog::error("Buffer data is empty");
    return std::unexpected{Error::kDataEmpty};
  }

  std::uint32_t handle{};
  ::glCreateBuffers(1, &handle);
  if (handle == 0) {
    spdlog::error("Failed to create buffer");
    return std::unexpected{Error::kCreateFailed};
  }

  const std::uint32_t flags{usage == Usage::kDynamic ? GL_DYNAMIC_STORAGE_BIT : 0U};
  ::glNamedBufferStorage(handle, static_cast<std::ptrdiff_t>(data.size()), data.data(), flags);

  return Buffer{handle, data.size(), usage};
}

auto Buffer::Reset() -> void {
  if (handle_ != 0) {
    ::glDeleteBuffers(1, &handle_);
    handle_ = 0;
  }
  size_ = 0;
  usage_ = Usage::kStatic;
}

auto Buffer::Update(std::span<const std::byte> data, std::size_t offset) const -> void {
  if (handle_ == 0) {
    spdlog::error("Buffer not initialized");
    return;
  }
  if (usage_ != Usage::kDynamic) {
    spdlog::error("Buffer is not dynamic");
    return;
  }
  if (offset > size_ || data.size() > size_ - offset) {
    spdlog::error("Buffer update out of range: offset {} + size {} > {}", offset, data.size(), size_);
    return;
  }

  ::glNamedBufferSubData(handle_, static_cast<std::ptrdiff_t>(offset), static_cast<std::ptrdiff_t>(data.size()),
                         data.data());
}
