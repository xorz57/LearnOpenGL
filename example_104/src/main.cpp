#include "camera.h"
#include "shader.h"
#include "vertex.h"

#include <SDL3/SDL.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <utility>

template <typename Function> class ScopeExit final {
public:
  explicit ScopeExit(Function function) : function_{std::move(function)} {}
  ~ScopeExit() { function_(); }

  ScopeExit(ScopeExit const &) = delete;
  auto operator=(ScopeExit const &) -> ScopeExit & = delete;

  ScopeExit(ScopeExit &&) = delete;
  auto operator=(ScopeExit &&) -> ScopeExit & = delete;

private:
  Function function_;
};

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto main() -> int {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    spdlog::error("SDL initialization failed: {}", SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL initialized successfully");
  auto sdl_cleanup{ScopeExit{[]() -> void { SDL_Quit(); }}};

  char const *glsl_version{"#version 460 core"};

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  float const main_scale{SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay())};

  char const *title{"example_104"};
  std::int32_t const window_w{static_cast<std::int32_t>(1'280 * main_scale)};
  std::int32_t const window_h{static_cast<std::int32_t>(720 * main_scale)};
  SDL_WindowFlags const flags{SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN |
                              SDL_WINDOW_HIGH_PIXEL_DENSITY};
  SDL_Window *const window{SDL_CreateWindow(title, window_w, window_h, flags)};
  if (window == nullptr) {
    spdlog::error("SDL window creation failed: {}", SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL window created successfully");
  auto window_cleanup{ScopeExit{[&]() -> void { SDL_DestroyWindow(window); }}};

  SDL_GLContext const gl_context{SDL_GL_CreateContext(window)};
  if (gl_context == nullptr) {
    spdlog::error("SDL GL context creation failed: {}", SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL GL context created successfully");
  auto gl_context_cleanup{ScopeExit{[&]() -> void { SDL_GL_DestroyContext(gl_context); }}};

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);
  SDL_SetWindowRelativeMouseMode(window, true);

  // NOLINTNEXTLINE
  if (gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)) == 0) {
    spdlog::error("GLAD initialization failed");
    return EXIT_FAILURE;
  }
  spdlog::info("GLAD initialized successfully");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("OpenGL version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
  spdlog::info("GLSL version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
  spdlog::info("Vendor: {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
  spdlog::info("Renderer: {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
  // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto imgui_cleanup{ScopeExit{[]() -> void {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
  }}};
  // NOLINTNEXTLINE(readability-identifier-length)
  ImGuiIO &io{ImGui::GetIO()};
  static_cast<void>(io);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  ImGuiStyle &style{ImGui::GetStyle()};
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;
  io.ConfigDpiScaleFonts = true;
  io.ConfigDpiScaleViewports = true;

  if (static_cast<bool>(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)) {
    style.WindowRounding = 0.0F;
    style.Colors[ImGuiCol_WindowBg].w = 1.0F;
  }

  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  std::vector<Vertex> vertices{
      // Front (+z)
      {.position = {-0.5F, -0.5F, 0.5F}, .normal = {0.0F, 0.0F, 1.0F}, .uv = {0.0F, 0.0F}},
      {.position = {0.5F, -0.5F, 0.5F}, .normal = {0.0F, 0.0F, 1.0F}, .uv = {1.0F, 0.0F}},
      {.position = {0.5F, 0.5F, 0.5F}, .normal = {0.0F, 0.0F, 1.0F}, .uv = {1.0F, 1.0F}},
      {.position = {-0.5F, 0.5F, 0.5F}, .normal = {0.0F, 0.0F, 1.0F}, .uv = {0.0F, 1.0F}},
      // Back (-z)
      {.position = {0.5F, -0.5F, -0.5F}, .normal = {0.0F, 0.0F, -1.0F}, .uv = {0.0F, 0.0F}},
      {.position = {-0.5F, -0.5F, -0.5F}, .normal = {0.0F, 0.0F, -1.0F}, .uv = {1.0F, 0.0F}},
      {.position = {-0.5F, 0.5F, -0.5F}, .normal = {0.0F, 0.0F, -1.0F}, .uv = {1.0F, 1.0F}},
      {.position = {0.5F, 0.5F, -0.5F}, .normal = {0.0F, 0.0F, -1.0F}, .uv = {0.0F, 1.0F}},
      // Left (-x)
      {.position = {-0.5F, -0.5F, -0.5F}, .normal = {-1.0F, 0.0F, 0.0F}, .uv = {0.0F, 0.0F}},
      {.position = {-0.5F, -0.5F, 0.5F}, .normal = {-1.0F, 0.0F, 0.0F}, .uv = {1.0F, 0.0F}},
      {.position = {-0.5F, 0.5F, 0.5F}, .normal = {-1.0F, 0.0F, 0.0F}, .uv = {1.0F, 1.0F}},
      {.position = {-0.5F, 0.5F, -0.5F}, .normal = {-1.0F, 0.0F, 0.0F}, .uv = {0.0F, 1.0F}},
      // Right (+x)
      {.position = {0.5F, -0.5F, 0.5F}, .normal = {1.0F, 0.0F, 0.0F}, .uv = {0.0F, 0.0F}},
      {.position = {0.5F, -0.5F, -0.5F}, .normal = {1.0F, 0.0F, 0.0F}, .uv = {1.0F, 0.0F}},
      {.position = {0.5F, 0.5F, -0.5F}, .normal = {1.0F, 0.0F, 0.0F}, .uv = {1.0F, 1.0F}},
      {.position = {0.5F, 0.5F, 0.5F}, .normal = {1.0F, 0.0F, 0.0F}, .uv = {0.0F, 1.0F}},
      // Top (+y)
      {.position = {-0.5F, 0.5F, 0.5F}, .normal = {0.0F, 1.0F, 0.0F}, .uv = {0.0F, 0.0F}},
      {.position = {0.5F, 0.5F, 0.5F}, .normal = {0.0F, 1.0F, 0.0F}, .uv = {1.0F, 0.0F}},
      {.position = {0.5F, 0.5F, -0.5F}, .normal = {0.0F, 1.0F, 0.0F}, .uv = {1.0F, 1.0F}},
      {.position = {-0.5F, 0.5F, -0.5F}, .normal = {0.0F, 1.0F, 0.0F}, .uv = {0.0F, 1.0F}},
      // Bottom (-y)
      {.position = {-0.5F, -0.5F, -0.5F}, .normal = {0.0F, -1.0F, 0.0F}, .uv = {0.0F, 0.0F}},
      {.position = {0.5F, -0.5F, -0.5F}, .normal = {0.0F, -1.0F, 0.0F}, .uv = {1.0F, 0.0F}},
      {.position = {0.5F, -0.5F, 0.5F}, .normal = {0.0F, -1.0F, 0.0F}, .uv = {1.0F, 1.0F}},
      {.position = {-0.5F, -0.5F, 0.5F}, .normal = {0.0F, -1.0F, 0.0F}, .uv = {0.0F, 1.0F}},
  };

  std::vector<uint32_t> indices{
      0,  1,  2,  2,  3,  0,  // Front
      4,  5,  6,  6,  7,  4,  // Back
      8,  9,  10, 10, 11, 8,  // Left
      12, 13, 14, 14, 15, 12, // Right
      16, 17, 18, 18, 19, 16, // Top
      20, 21, 22, 22, 23, 20, // Bottom
  };

  std::vector<glm::mat4> models{
      glm::translate(glm::mat4(1.0F), {-1.0F, 2.0F, -1.0F}),
      glm::translate(glm::mat4(1.0F), {1.0F, 2.0F, 1.0F}),
      glm::scale(glm::mat4(1.0F), {5.0F, 1.0F, 5.0F}),
  };

  std::uint32_t vbo{};
  std::uint32_t ebo{};
  std::uint32_t instance_buffer{};

  std::uint32_t vao{};

  glCreateBuffers(1, &vbo);
  glCreateBuffers(1, &ebo);
  glCreateBuffers(1, &instance_buffer);
  auto vbo_cleanup{ScopeExit{[&]() -> void { glDeleteBuffers(1, &vbo); }}};
  auto ebo_cleanup{ScopeExit{[&]() -> void { glDeleteBuffers(1, &ebo); }}};
  auto instance_buffer_cleanup{ScopeExit{[&]() -> void { glDeleteBuffers(1, &instance_buffer); }}};

  glCreateVertexArrays(1, &vao);
  auto vao_cleanup{ScopeExit{[&]() -> void { glDeleteVertexArrays(1, &vao); }}};

  glNamedBufferStorage(vbo, static_cast<std::ptrdiff_t>(vertices.size() * sizeof(Vertex)), vertices.data(), 0);
  glNamedBufferStorage(ebo, static_cast<std::ptrdiff_t>(indices.size() * sizeof(std::uint32_t)), indices.data(), 0);
  glNamedBufferStorage(instance_buffer, static_cast<std::ptrdiff_t>(models.size() * sizeof(glm::mat4)), models.data(), 0);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
  glVertexArrayVertexBuffer(vao, 1, instance_buffer, 0, sizeof(glm::mat4));
  glVertexArrayElementBuffer(vao, ebo);

  glEnableVertexArrayAttrib(vao, 0);
  glEnableVertexArrayAttrib(vao, 1);
  glEnableVertexArrayAttrib(vao, 2);
  glEnableVertexArrayAttrib(vao, 3);
  glEnableVertexArrayAttrib(vao, 4);
  glEnableVertexArrayAttrib(vao, 5);
  glEnableVertexArrayAttrib(vao, 6);

  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
  glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao, 4, 4, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec4));
  glVertexArrayAttribFormat(vao, 5, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4));
  glVertexArrayAttribFormat(vao, 6, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec4));

  // glVertexArrayBindingDivisor(vao, 0, 0);
  glVertexArrayBindingDivisor(vao, 1, 1);

  // glVertexArrayAttribBinding(vao, 0, 0);
  // glVertexArrayAttribBinding(vao, 1, 0);
  // glVertexArrayAttribBinding(vao, 2, 0);
  glVertexArrayAttribBinding(vao, 3, 1);
  glVertexArrayAttribBinding(vao, 4, 1);
  glVertexArrayAttribBinding(vao, 5, 1);
  glVertexArrayAttribBinding(vao, 6, 1);

  std::optional<Shader> shader{
      Shader::loadFromFile(ASSETS_DIR "shaders/unlit_color.vert.glsl", ASSETS_DIR "shaders/unlit_color.frag.glsl")};
  if (!shader.has_value()) {
    return EXIT_FAILURE;
  }

  Camera camera{};

  std::uint64_t last_frame_ticks{SDL_GetTicks()};

  bool done{false};

  while (!done) {
    std::uint64_t const current_frame_ticks{SDL_GetTicks()};
    float const delta_time{static_cast<float>(current_frame_ticks - last_frame_ticks) / 1'000.0F};
    last_frame_ticks = current_frame_ticks;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      switch (event.type) {
      case SDL_EVENT_QUIT:
        done = true;
        break;
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        if (event.window.windowID == SDL_GetWindowID(window)) {
          done = true;
        }
        break;
      case SDL_EVENT_MOUSE_MOTION:
        if (!io.WantCaptureMouse) {
          camera.processMouseMovement(event.motion.xrel, -event.motion.yrel);
        }
        break;
      case SDL_EVENT_KEY_DOWN:
        if (!event.key.repeat && event.key.key == SDLK_RETURN && static_cast<bool>(event.key.mod & SDL_KMOD_ALT)) {
          bool const is_fullscreen{static_cast<bool>(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN)};
          SDL_SetWindowFullscreen(window, !is_fullscreen);
        }
        break;
      default:
        break;
      }
    }

    if (static_cast<bool>(SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) {
      SDL_Delay(10);
      continue;
    }

    if (!io.WantCaptureKeyboard) {
      bool const *keyboard_state{SDL_GetKeyboardState(nullptr)};
      // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      float const speed_multiplier{(keyboard_state[SDL_SCANCODE_LSHIFT]) ? 2.5F : 1.0F};
      if (keyboard_state[SDL_SCANCODE_W]) {
        camera.processKeyboard(Camera::Movement::Forward, delta_time, speed_multiplier);
      }
      if (keyboard_state[SDL_SCANCODE_S]) {
        camera.processKeyboard(Camera::Movement::Backward, delta_time, speed_multiplier);
      }
      if (keyboard_state[SDL_SCANCODE_A]) {
        camera.processKeyboard(Camera::Movement::Left, delta_time, speed_multiplier);
      }
      if (keyboard_state[SDL_SCANCODE_D]) {
        camera.processKeyboard(Camera::Movement::Right, delta_time, speed_multiplier);
      }
      if (keyboard_state[SDL_SCANCODE_SPACE]) {
        camera.processKeyboard(Camera::Movement::Up, delta_time, speed_multiplier);
      }
      if (keyboard_state[SDL_SCANCODE_LCTRL]) {
        camera.processKeyboard(Camera::Movement::Down, delta_time, speed_multiplier);
      }
      // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Render();
    std::int32_t const framebuffer_w{
        static_cast<std::int32_t>(std::round(io.DisplaySize.x * io.DisplayFramebufferScale.x))};
    std::int32_t const framebuffer_h{
        static_cast<std::int32_t>(std::round(io.DisplaySize.y * io.DisplayFramebufferScale.y))};
    glViewport(0, 0, framebuffer_w, framebuffer_h);
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float const framebuffer_a{static_cast<float>(framebuffer_w) / static_cast<float>(framebuffer_h)};

    glm::mat4 const projection{camera.computeProjectionMatrix(framebuffer_a)};
    glm::mat4 const view{camera.computeViewMatrix()};

    shader->use();
    shader->setUniform("u_projection", projection);
    shader->setUniform("u_view", view);

    shader->setUniform("u_color", glm::vec3{1.0F, 0.0F, 0.0F});
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES,
                   static_cast<std::int32_t>(indices.size()),
                   GL_UNSIGNED_INT,
                   static_cast<void *>(nullptr),
                  static_cast<std::int32_t>(models.size()));

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (static_cast<bool>(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)) {
      SDL_Window *const backup_current_window{SDL_GL_GetCurrentWindow()};
      SDL_GLContext const backup_current_context{SDL_GL_GetCurrentContext()};
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

    SDL_GL_SwapWindow(window);
  }

  return EXIT_SUCCESS;
}
