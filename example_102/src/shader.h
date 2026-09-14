#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

class Shader final {
public:
  enum class ShaderType : std::uint8_t {
    Vertex,
    Fragment,
  };

  ~Shader() noexcept;

  Shader(const Shader &) = delete;
  auto operator=(const Shader &) -> Shader & = delete;

  Shader(Shader &&other) noexcept;
  auto operator=(Shader &&other) noexcept -> Shader &;

  [[nodiscard]] static auto loadFromSource(char const *vertex_shader_source, char const *fragment_shader_source)
      -> std::optional<Shader>;
  [[nodiscard]] static auto loadFromFile(std::filesystem::path const &vertex_shader_path,
                                         std::filesystem::path const &fragment_shader_path) -> std::optional<Shader>;

  void use() const;

  void setUniform(char const *name, bool value) const;

  // NOLINTBEGIN(readability-identifier-length)

  void setUniform(char const *name, float v0) const;
  void setUniform(char const *name, float v0, float v1) const;
  void setUniform(char const *name, float v0, float v1, float v2) const;
  void setUniform(char const *name, float v0, float v1, float v2, float v3) const;

  void setUniform(char const *name, std::int32_t v0) const;
  void setUniform(char const *name, std::int32_t v0, std::int32_t v1) const;
  void setUniform(char const *name, std::int32_t v0, std::int32_t v1, std::int32_t v2) const;
  void setUniform(char const *name, std::int32_t v0, std::int32_t v1, std::int32_t v2, std::int32_t v3) const;

  void setUniform(char const *name, std::uint32_t v0) const;
  void setUniform(char const *name, std::uint32_t v0, std::uint32_t v1) const;
  void setUniform(char const *name, std::uint32_t v0, std::uint32_t v1, std::uint32_t v2) const;
  void setUniform(char const *name, std::uint32_t v0, std::uint32_t v1, std::uint32_t v2, std::uint32_t v3) const;

  // NOLINTEND(readability-identifier-length)

  void setUniform(char const *name, glm::vec2 const &value) const;
  void setUniform(char const *name, glm::vec3 const &value) const;
  void setUniform(char const *name, glm::vec4 const &value) const;

  void setUniform(char const *name, glm::ivec2 const &value) const;
  void setUniform(char const *name, glm::ivec3 const &value) const;
  void setUniform(char const *name, glm::ivec4 const &value) const;

  void setUniform(char const *name, glm::uvec2 const &value) const;
  void setUniform(char const *name, glm::uvec3 const &value) const;
  void setUniform(char const *name, glm::uvec4 const &value) const;

  void setUniform(char const *name, glm::mat2 const &value) const;
  void setUniform(char const *name, glm::mat3 const &value) const;
  void setUniform(char const *name, glm::mat4 const &value) const;

private:
  struct StringHash final {
    // NOLINTNEXTLINE(readability-identifier-naming)
    using is_transparent = void;

    auto operator()(char const *str) const noexcept -> std::size_t { return std::hash<std::string_view>{}(str); }
    auto operator()(std::string_view str) const noexcept -> std::size_t { return std::hash<std::string_view>{}(str); }
    auto operator()(std::string const &str) const noexcept -> std::size_t { return std::hash<std::string_view>{}(str); }
  };

  Shader();

  [[nodiscard]] auto build(char const *vertex_shader_source, char const *fragment_shader_source) -> bool;
  [[nodiscard]] static auto verifyShaderCompiled(std::uint32_t shader, ShaderType type) -> bool;
  [[nodiscard]] static auto verifyProgramLinked(std::uint32_t program) -> bool;
  void cacheUniformLocations();
  [[nodiscard]] auto getUniformLocation(char const *name) const -> std::int32_t;

  std::uint32_t id_{};
  std::unordered_map<std::string, std::int32_t, StringHash, std::equal_to<>> uniform_location_cache_;
};
