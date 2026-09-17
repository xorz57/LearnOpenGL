#include <SDL3/SDL.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <cmath>
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

  char const *title{"example_101"};
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

  // NOLINTNEXTLINE
  if (gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)) == 0) {
    spdlog::error("GLAD initialization failed");
    return EXIT_FAILURE;
  }
  spdlog::info("GLAD initialized successfully");

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

  bool done{false};

  while (!done) {
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
      default:
        break;
      }
    }

    if (static_cast<bool>(SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) {
      SDL_Delay(10);
      continue;
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
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO

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
