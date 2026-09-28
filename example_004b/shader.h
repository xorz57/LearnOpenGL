#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>

class Shader final {
 public:
  enum class Type : std::uint8_t {
    kVertex,
    kFragment,
  };

  enum class Error : std::uint8_t {
    kFileOpenFailed,
    kFileEmpty,
    kCompileFailed,
  };

  ~Shader();

  Shader(const Shader&) = delete;
  auto operator=(const Shader&) -> Shader& = delete;

  Shader(Shader&& other) noexcept;
  auto operator=(Shader&& other) noexcept -> Shader&;

  [[nodiscard]] static auto Create(Type type, const char* source) -> std::expected<Shader, Error>;
  [[nodiscard]] static auto CreateFromFile(Type type, const std::filesystem::path& path) -> std::expected<Shader, Error>;

  auto Reset() noexcept -> void;

  [[nodiscard]] auto GetHandle() const noexcept -> std::uint32_t { return handle_; }

 private:
  explicit Shader(std::uint32_t handle) noexcept : handle_{handle} {}

  std::uint32_t handle_{};
};
