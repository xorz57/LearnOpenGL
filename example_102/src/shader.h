#pragma once

#include <glm/glm.hpp>

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

  void setBool(char const *name, bool value) const;
  void setInt(char const *name, std::int32_t value) const;
  void setFloat(char const *name, float value) const;

  void setVec2(char const *name, glm::vec2 const &value) const;
  void setVec3(char const *name, glm::vec3 const &value) const;
  void setVec4(char const *name, glm::vec4 const &value) const;

  void setMat2(char const *name, glm::mat2 const &mat) const;
  void setMat3(char const *name, glm::mat3 const &mat) const;
  void setMat4(char const *name, glm::mat4 const &mat) const;

private:
  struct StringHash final {
    // NOLINTNEXTLINE(readability-identifier-naming)
    using is_transparent = void;

    auto operator()(char const *str) const noexcept -> std::size_t { return std::hash<std::string_view>{}(str); }
    auto operator()(std::string_view str) const noexcept -> std::size_t { return std::hash<std::string_view>{}(str); }
    auto operator()(std::string const &str) const noexcept -> std::size_t { return std::hash<std::string_view>{}(str); }
  };

  Shader();

  [[nodiscard]] auto getUniformLocation(char const *name) const -> std::int32_t;
  [[nodiscard]] auto compile(char const *vertex_shader_source, char const *fragment_shader_source) -> bool;
  [[nodiscard]] static auto checkCompileErrors(std::uint32_t shader, ShaderType type) -> bool;
  [[nodiscard]] static auto checkLinkErrors(std::uint32_t program) -> bool;

  std::uint32_t id_{};
  mutable std::unordered_map<std::string, std::int32_t, StringHash, std::equal_to<>> uniform_location_cache_;
};
