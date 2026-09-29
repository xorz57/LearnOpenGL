#include "camera.h"
#include "instance.h"
#include "program.h"
#include "scope_exit.h"
#include "shader.h"
#include "vertex.h"

#include <SDL3/SDL.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <vector>

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto main() -> int {
  if (!::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    spdlog::error("Failed to initialize SDL: {}", ::SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL initialized successfully");
  auto sdl_cleanup{ScopeExit{[] -> void { ::SDL_Quit(); }}};

  const char* glsl_version{"#version 460 core"};

  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_FLAGS, 0);
  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_MINOR_VERSION, 6);

  ::SDL_GL_SetAttribute(::SDL_GL_DOUBLEBUFFER, 1);
  ::SDL_GL_SetAttribute(::SDL_GL_DEPTH_SIZE, 24);
  ::SDL_GL_SetAttribute(::SDL_GL_STENCIL_SIZE, 8);

  float main_scale{::SDL_GetDisplayContentScale(::SDL_GetPrimaryDisplay())};
  if (main_scale == 0.0F) {
    spdlog::warn("Failed to get display content scale: {}", ::SDL_GetError());
    main_scale = 1.0F;
  }

  const char* title{"example_003b"};
  const auto window_w{static_cast<std::int32_t>(1'280 * main_scale)};
  const auto window_h{static_cast<std::int32_t>(720 * main_scale)};
  const ::SDL_WindowFlags flags{SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY};
  ::SDL_Window* const window{::SDL_CreateWindow(title, window_w, window_h, flags)};
  if (window == nullptr) {
    spdlog::error("Failed to create SDL window: {}", ::SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL window created successfully");
  auto window_cleanup{ScopeExit{[&] -> void { ::SDL_DestroyWindow(window); }}};

  const ::SDL_GLContext gl_context{::SDL_GL_CreateContext(window)};
  if (gl_context == nullptr) {
    spdlog::error("Failed to create SDL GL context: {}", ::SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL GL context created successfully");
  auto gl_context_cleanup{ScopeExit{[&] -> void { ::SDL_GL_DestroyContext(gl_context); }}};

  ::SDL_GL_MakeCurrent(window, gl_context);
  ::SDL_GL_SetSwapInterval(1);
  ::SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  ::SDL_ShowWindow(window);
  ::SDL_SetWindowRelativeMouseMode(window, true);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (::gladLoadGL(reinterpret_cast<::GLADloadfunc>(::SDL_GL_GetProcAddress)) == 0) {
    spdlog::error("Failed to initialize GLAD");
    return EXIT_FAILURE;
  }
  spdlog::info("GLAD initialized successfully");

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("OpenGL version: {}", reinterpret_cast<const char*>(::glGetString(GL_VERSION)));
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("GLSL version: {}", reinterpret_cast<const char*>(::glGetString(GL_SHADING_LANGUAGE_VERSION)));
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("Vendor: {}", reinterpret_cast<const char*>(::glGetString(GL_VENDOR)));
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("Renderer: {}", reinterpret_cast<const char*>(::glGetString(GL_RENDERER)));

  ::glEnable(GL_DEPTH_TEST);
  ::glEnable(GL_CULL_FACE);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto imgui_cleanup{ScopeExit{[] -> void {
    ::ImGui_ImplOpenGL3_Shutdown();
    ::ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
  }}};
  ::ImGuiIO& io{ImGui::GetIO()};
  io.ConfigFlags |= ::ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ::ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ::ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ::ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  ::ImGuiStyle& style{ImGui::GetStyle()};
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;
  io.ConfigDpiScaleFonts = true;
  io.ConfigDpiScaleViewports = true;

  if (static_cast<bool>(io.ConfigFlags & ::ImGuiConfigFlags_ViewportsEnable)) {
    style.WindowRounding = 0.0F;
    style.Colors[::ImGuiCol_WindowBg].w = 1.0F;
  }

  ::ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ::ImGui_ImplOpenGL3_Init(glsl_version);

  const std::vector<Vertex> vertices{
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

  // clang-format off
  const std::vector<std::uint32_t> indices{
     0,  1,  2,  2,  3,  0,  // Front
     4,  5,  6,  6,  7,  4,  // Back
     8,  9, 10, 10, 11,  8,  // Left
    12, 13, 14, 14, 15, 12,  // Right
    16, 17, 18, 18, 19, 16,  // Top
    20, 21, 22, 22, 23, 20,  // Bottom
  };
  // clang-format on

  const std::vector<Instance> instances{
      {.model = glm::translate(glm::mat4{1.0F}, {-1.0F, 2.0F, -1.0F}), .color = {1.0F, 0.0F, 0.0F}},
      {.model = glm::translate(glm::mat4{1.0F}, {1.0F, 2.0F, 1.0F}), .color = {0.0F, 1.0F, 0.0F}},
      {.model = glm::scale(glm::mat4{1.0F}, {4.0F, 1.0F, 4.0F}), .color = {0.0F, 0.0F, 1.0F}},
  };

  std::uint32_t vbo{};
  std::uint32_t ebo{};
  std::uint32_t ibo{};

  std::uint32_t vao{};

  ::glCreateBuffers(1, &vbo);
  ::glCreateBuffers(1, &ebo);
  ::glCreateBuffers(1, &ibo);
  auto vbo_cleanup{ScopeExit{[&] -> void { ::glDeleteBuffers(1, &vbo); }}};
  auto ebo_cleanup{ScopeExit{[&] -> void { ::glDeleteBuffers(1, &ebo); }}};
  auto ibo_cleanup{ScopeExit{[&] -> void { ::glDeleteBuffers(1, &ibo); }}};

  ::glCreateVertexArrays(1, &vao);
  auto vao_cleanup{ScopeExit{[&] -> void { ::glDeleteVertexArrays(1, &vao); }}};

  ::glNamedBufferStorage(vbo, static_cast<std::ptrdiff_t>(vertices.size() * sizeof(Vertex)), vertices.data(), 0);
  ::glNamedBufferStorage(ebo, static_cast<std::ptrdiff_t>(indices.size() * sizeof(std::uint32_t)), indices.data(), 0);
  ::glNamedBufferStorage(ibo, static_cast<std::ptrdiff_t>(instances.size() * sizeof(Instance)), instances.data(), 0);

  ::glVertexArrayVertexBuffer(vao, 0, vbo, 0, static_cast<std::int32_t>(sizeof(Vertex)));
  ::glVertexArrayVertexBuffer(vao, 1, ibo, 0, static_cast<std::int32_t>(sizeof(Instance)));
  ::glVertexArrayElementBuffer(vao, ebo);

  ::glEnableVertexArrayAttrib(vao, 0);
  ::glEnableVertexArrayAttrib(vao, 1);
  ::glEnableVertexArrayAttrib(vao, 2);
  ::glEnableVertexArrayAttrib(vao, 3);
  ::glEnableVertexArrayAttrib(vao, 4);
  ::glEnableVertexArrayAttrib(vao, 5);
  ::glEnableVertexArrayAttrib(vao, 6);
  ::glEnableVertexArrayAttrib(vao, 7);

  ::glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  ::glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  ::glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
  ::glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (0 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 4, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (1 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 5, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (2 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 6, 4, GL_FLOAT, GL_FALSE, offsetof(Instance, model) + (3 * sizeof(glm::vec4)));
  ::glVertexArrayAttribFormat(vao, 7, 3, GL_FLOAT, GL_FALSE, offsetof(Instance, color));

  ::glVertexArrayBindingDivisor(vao, 0, 0);
  ::glVertexArrayBindingDivisor(vao, 1, 1);

  ::glVertexArrayAttribBinding(vao, 0, 0);
  ::glVertexArrayAttribBinding(vao, 1, 0);
  ::glVertexArrayAttribBinding(vao, 2, 0);
  ::glVertexArrayAttribBinding(vao, 3, 1);
  ::glVertexArrayAttribBinding(vao, 4, 1);
  ::glVertexArrayAttribBinding(vao, 5, 1);
  ::glVertexArrayAttribBinding(vao, 6, 1);
  ::glVertexArrayAttribBinding(vao, 7, 1);

  const char* base_path{::SDL_GetBasePath()};
  if (base_path == nullptr) {
    spdlog::error("Failed to get base path: {}", ::SDL_GetError());
    return EXIT_FAILURE;
  }
  const std::filesystem::path assets_dir{std::filesystem::path{base_path} / "assets"};

  auto unlit_vertex_shader{Shader::CreateFromFile(Shader::Type::kVertex, assets_dir / "shaders/unlit.vert.glsl")};
  if (!unlit_vertex_shader.has_value()) {
    return EXIT_FAILURE;
  }

  auto unlit_fragment_shader{Shader::CreateFromFile(Shader::Type::kFragment, assets_dir / "shaders/unlit.frag.glsl")};
  if (!unlit_fragment_shader.has_value()) {
    return EXIT_FAILURE;
  }

  auto unlit_program{Program::Create(*unlit_vertex_shader, *unlit_fragment_shader)};
  if (!unlit_program.has_value()) {
    return EXIT_FAILURE;
  }

  unlit_vertex_shader->Reset();
  unlit_fragment_shader->Reset();

  Camera camera{{0.0F, 2.0F, 10.0F}};

  std::uint64_t last_frame_ticks{::SDL_GetTicks()};

  bool done{false};

  while (!done) {
    const std::uint64_t current_frame_ticks{::SDL_GetTicks()};
    const float delta_time{static_cast<float>(current_frame_ticks - last_frame_ticks) / 1'000.0F};
    last_frame_ticks = current_frame_ticks;

    ::SDL_Event event{};
    while (::SDL_PollEvent(&event)) {
      ::ImGui_ImplSDL3_ProcessEvent(&event);
      switch (event.type) {
        case ::SDL_EVENT_QUIT:
          done = true;
          break;
        case ::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          if (event.window.windowID == ::SDL_GetWindowID(window)) {
            done = true;
          }
          break;
        case ::SDL_EVENT_MOUSE_MOTION:
          if (!io.WantCaptureMouse) {
            camera.ProcessMouseMovement(event.motion.xrel, -event.motion.yrel);
          }
          break;
        case ::SDL_EVENT_KEY_DOWN:
          if (!event.key.repeat && event.key.key == SDLK_RETURN && static_cast<bool>(event.key.mod & SDL_KMOD_ALT)) {
            const bool is_fullscreen{static_cast<bool>(::SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN)};
            ::SDL_SetWindowFullscreen(window, !is_fullscreen);
          }
          break;
        default:
          break;
      }
    }

    if (static_cast<bool>(::SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) {
      ::SDL_Delay(10);
      continue;
    }

    if (!io.WantCaptureKeyboard) {
      const bool* keyboard_state{::SDL_GetKeyboardState(nullptr)};
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const float speed_multiplier{keyboard_state[::SDL_SCANCODE_LSHIFT] ? 2.5F : 1.0F};
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      if (keyboard_state[::SDL_SCANCODE_W]) {
        camera.ProcessKeyboard(Camera::Movement::kForward, delta_time, speed_multiplier);
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      if (keyboard_state[::SDL_SCANCODE_S]) {
        camera.ProcessKeyboard(Camera::Movement::kBackward, delta_time, speed_multiplier);
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      if (keyboard_state[::SDL_SCANCODE_A]) {
        camera.ProcessKeyboard(Camera::Movement::kLeft, delta_time, speed_multiplier);
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      if (keyboard_state[::SDL_SCANCODE_D]) {
        camera.ProcessKeyboard(Camera::Movement::kRight, delta_time, speed_multiplier);
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      if (keyboard_state[::SDL_SCANCODE_SPACE]) {
        camera.ProcessKeyboard(Camera::Movement::kUp, delta_time, speed_multiplier);
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      if (keyboard_state[::SDL_SCANCODE_LCTRL]) {
        camera.ProcessKeyboard(Camera::Movement::kDown, delta_time, speed_multiplier);
      }
    }

    ::ImGui_ImplOpenGL3_NewFrame();
    ::ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Render();
    const auto framebuffer_w{static_cast<std::int32_t>(std::round(io.DisplaySize.x * io.DisplayFramebufferScale.x))};
    const auto framebuffer_h{static_cast<std::int32_t>(std::round(io.DisplaySize.y * io.DisplayFramebufferScale.y))};
    ::glViewport(0, 0, framebuffer_w, framebuffer_h);
    ::glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspect_ratio{static_cast<float>(framebuffer_w) / static_cast<float>(framebuffer_h)};

    const glm::mat4 projection{Camera::ComputeProjectionMatrix(aspect_ratio)};
    const glm::mat4 view{camera.ComputeViewMatrix()};

    unlit_program->Use();
    unlit_program->SetUniform("u_projection", projection);
    unlit_program->SetUniform("u_view", view);

    ::glBindVertexArray(vao);
    ::glDrawElementsInstanced(GL_TRIANGLES, static_cast<std::int32_t>(indices.size()), GL_UNSIGNED_INT, static_cast<void*>(nullptr), static_cast<std::int32_t>(instances.size()));

    ::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (static_cast<bool>(io.ConfigFlags & ::ImGuiConfigFlags_ViewportsEnable)) {
      ::SDL_Window* const backup_current_window{::SDL_GL_GetCurrentWindow()};
      const ::SDL_GLContext backup_current_context{::SDL_GL_GetCurrentContext()};
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      ::SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

    ::SDL_GL_SwapWindow(window);
  }

  return EXIT_SUCCESS;
}
