#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>

class Buffer final {
public:
  enum class Usage : std::uint8_t {
    kStatic,
    kDynamic,
  };

  enum class Error : std::uint8_t {
    kDataEmpty,
    kCreateFailed,
  };

  ~Buffer();

  Buffer(const Buffer &) = delete;
  auto operator=(const Buffer &) -> Buffer & = delete;

  Buffer(Buffer &&other) noexcept;
  auto operator=(Buffer &&other) noexcept -> Buffer &;

  [[nodiscard]] static auto Create(std::span<const std::byte> data, Usage usage = Usage::kStatic)
      -> std::expected<Buffer, Error>;

  auto Reset() -> void;

  auto Update(std::span<const std::byte> data, std::size_t offset = 0) const -> void;

  [[nodiscard]] auto GetHandle() const -> std::uint32_t { return handle_; }
  [[nodiscard]] auto GetSize() const -> std::size_t { return size_; }
  [[nodiscard]] auto GetUsage() const -> Usage { return usage_; }

private:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  explicit Buffer(std::uint32_t handle, std::size_t size, Usage usage) : handle_{handle}, size_{size}, usage_{usage} {}

  std::uint32_t handle_{};
  std::size_t size_{};
  Usage usage_{Usage::kStatic};
};
