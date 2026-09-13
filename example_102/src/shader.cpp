#include "shader.h"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <utility>

Shader::Shader() = default;

Shader::~Shader() noexcept { glDeleteProgram(id_); }

Shader::Shader(Shader &&other) noexcept
    : id_(std::exchange(other.id_, 0)), uniform_location_cache_(std::move(other.uniform_location_cache_)) {}

auto Shader::operator=(Shader &&other) noexcept -> Shader & {
  if (this != &other) {
    glDeleteProgram(id_);
    id_ = std::exchange(other.id_, 0);
    uniform_location_cache_ = std::move(other.uniform_location_cache_);
  }
  return *this;
}

auto Shader::loadFromSource(char const *vertex_shader_source, char const *fragment_shader_source)
    -> std::optional<Shader> {
  Shader shader{};
  if (!shader.compile(vertex_shader_source, fragment_shader_source)) {
    spdlog::error("Failed to compile or link shader program");
    return std::nullopt;
  }
  return shader;
}

auto Shader::loadFromFile(std::filesystem::path const &vertex_shader_path,
                          std::filesystem::path const &fragment_shader_path) -> std::optional<Shader> {
  auto read_file{[](std::filesystem::path const &path) -> std::optional<std::string> {
    std::ifstream const file(path, std::ios::in | std::ios::binary);
    if (!file) {
      return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }};

  auto const vertex_shader_source{read_file(vertex_shader_path)};
  auto const fragment_shader_source{read_file(fragment_shader_path)};

  if (!vertex_shader_source.has_value() || !fragment_shader_source.has_value()) {
    spdlog::error("Failed to open shader file");
    return std::nullopt;
  }

  if (vertex_shader_source->empty() || fragment_shader_source->empty()) {
    spdlog::error("Shader file is empty");
    return std::nullopt;
  }

  return loadFromSource(vertex_shader_source->c_str(), fragment_shader_source->c_str());
}

void Shader::use() const {
  if (id_ == 0) {
    spdlog::error("Shader program not initialized");
    glUseProgram(0);
    return;
  }

  glUseProgram(id_);
}

void Shader::setBool(char const *name, bool value) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniform1i(id_, loc, static_cast<std::int32_t>(value));
  }
}

void Shader::setInt(char const *name, std::int32_t value) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniform1i(id_, loc, value);
  }
}

void Shader::setFloat(char const *name, float value) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniform1f(id_, loc, value);
  }
}

void Shader::setVec2(char const *name, glm::vec2 const &value) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniform2fv(id_, loc, 1, glm::value_ptr(value));
  }
}

void Shader::setVec3(char const *name, glm::vec3 const &value) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniform3fv(id_, loc, 1, glm::value_ptr(value));
  }
}

void Shader::setVec4(char const *name, glm::vec4 const &value) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniform4fv(id_, loc, 1, glm::value_ptr(value));
  }
}

void Shader::setMat2(char const *name, glm::mat2 const &mat) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniformMatrix2fv(id_, loc, 1, GL_FALSE, glm::value_ptr(mat));
  }
}

void Shader::setMat3(char const *name, glm::mat3 const &mat) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniformMatrix3fv(id_, loc, 1, GL_FALSE, glm::value_ptr(mat));
  }
}

void Shader::setMat4(char const *name, glm::mat4 const &mat) const {
  std::int32_t const loc{getUniformLocation(name)};
  if (loc != -1) {
    glProgramUniformMatrix4fv(id_, loc, 1, GL_FALSE, glm::value_ptr(mat));
  }
}

auto Shader::getUniformLocation(char const *name) const -> std::int32_t {
  if (id_ == 0) {
    spdlog::error("Shader program not initialized");
    return -1;
  }

  // NOLINTNEXTLINE(readability-identifier-length)
  if (auto const it{uniform_location_cache_.find(name)}; it != uniform_location_cache_.end()) {
    return it->second;
  }

  std::int32_t const location{glGetUniformLocation(id_, name)};
  uniform_location_cache_.try_emplace(name, location);
  if (location == -1) {
    spdlog::warn("Shader uniform not found: {}", name);
  }
  return location;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,-warnings-as-errors)
auto Shader::compile(char const *vertex_shader_source, char const *fragment_shader_source) -> bool {
  std::uint32_t const vertex_shader{glCreateShader(GL_VERTEX_SHADER)};
  glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
  glCompileShader(vertex_shader);
  bool const vertex_compiled{checkCompileErrors(vertex_shader, ShaderType::Vertex)};

  std::uint32_t const fragment_shader{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
  glCompileShader(fragment_shader);
  bool const fragment_compiled{checkCompileErrors(fragment_shader, ShaderType::Fragment)};

  if (!vertex_compiled || !fragment_compiled) {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return false;
  }

  id_ = glCreateProgram();
  glAttachShader(id_, vertex_shader);
  glAttachShader(id_, fragment_shader);
  glLinkProgram(id_);
  if (!checkLinkErrors(id_)) {
    glDeleteProgram(id_);
    id_ = 0;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return id_ != 0;
}

auto Shader::checkCompileErrors(std::uint32_t shader, ShaderType type) -> bool {
  std::int32_t success{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    std::int32_t log_length{};
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    std::string info_log(static_cast<std::size_t>(log_length), '\0');
    glGetShaderInfoLog(shader, log_length, nullptr, info_log.data());
    switch (type) {
    case ShaderType::Vertex:
      spdlog::error("Shader compilation error [VERTEX]: {}", info_log);
      break;
    case ShaderType::Fragment:
      spdlog::error("Shader compilation error [FRAGMENT]: {}", info_log);
      break;
    }
    return false;
  }
  return true;
}

auto Shader::checkLinkErrors(std::uint32_t program) -> bool {
  std::int32_t success{};
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == 0) {
    std::int32_t log_length{};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    std::string info_log(static_cast<std::size_t>(log_length), '\0');
    glGetProgramInfoLog(program, log_length, nullptr, info_log.data());
    spdlog::error("Program linking error [PROGRAM]: {}", info_log);
    return false;
  }
  return true;
}
