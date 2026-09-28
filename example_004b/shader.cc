#include "shader.h"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <utility>

Shader::~Shader() { Reset(); }

Shader::Shader(Shader&& other) noexcept : handle_{std::exchange(other.handle_, 0)} {}

auto Shader::operator=(Shader&& other) noexcept -> Shader& {
  if (this != &other) {
    Reset();
    handle_ = std::exchange(other.handle_, 0);
  }
  return *this;
}

auto Shader::Create(Type type, const char* source) -> std::expected<Shader, Error> {
  const std::uint32_t handle{::glCreateShader(type == Type::kVertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER)};
  ::glShaderSource(handle, 1, &source, nullptr);
  ::glCompileShader(handle);

  std::int32_t success{};
  ::glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    std::int32_t info_log_length{};
    ::glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &info_log_length);
    std::string info_log(static_cast<std::size_t>(info_log_length), '\0');
    ::glGetShaderInfoLog(handle, info_log_length, nullptr, info_log.data());
    if (info_log_length > 0) {
      info_log.resize(static_cast<std::size_t>(info_log_length) - 1);
    }

    ::glDeleteShader(handle);
    spdlog::error("Failed to compile {} shader: {}", type == Type::kVertex ? "vertex" : "fragment", info_log);
    return std::unexpected{Error::kCompileFailed};
  }

  return Shader{handle};
}

auto Shader::CreateFromFile(Type type, const std::filesystem::path& path) -> std::expected<Shader, Error> {
  const std::ifstream file{path, std::ios::in | std::ios::binary};
  if (!file) {
    spdlog::error("Failed to open shader file: {}", path.string());
    return std::unexpected{Error::kFileOpenFailed};
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  const std::string source{buffer.str()};

  if (source.empty()) {
    spdlog::error("Shader file is empty: {}", path.string());
    return std::unexpected{Error::kFileEmpty};
  }

  return Create(type, source.c_str());
}

auto Shader::Reset() noexcept -> void {
  if (handle_ != 0) {
    ::glDeleteShader(handle_);
    handle_ = 0;
  }
}
