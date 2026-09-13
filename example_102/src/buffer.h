#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

class Buffer final {
public:
  template <typename T>
  explicit Buffer(std::span<T> data) : Buffer(data.data(), static_cast<std::ptrdiff_t>(data.size_bytes())) {}

  ~Buffer() noexcept;

  Buffer(Buffer const &) = delete;
  auto operator=(Buffer const &) -> Buffer & = delete;

  Buffer(Buffer &&other) noexcept;
  auto operator=(Buffer &&other) noexcept -> Buffer &;

  [[nodiscard]] auto getId() const noexcept -> std::uint32_t;

private:
  Buffer(void const *data, std::ptrdiff_t size);

  std::uint32_t id_{};
};
