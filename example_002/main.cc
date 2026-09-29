#include "scope_exit.h"

#include <SDL3/SDL.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <glad/gl.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>

auto main() -> int {
  if (!::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    spdlog::error("Failed to initialize SDL: {}", ::SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL initialized successfully");
  auto sdl_cleanup{ScopeExit{[] -> void { ::SDL_Quit(); }}};

  const char *glsl_version{"#version 460 core"};

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

  const char *title{"example_002"};
  const auto window_w{static_cast<std::int32_t>(1'280 * main_scale)};
  const auto window_h{static_cast<std::int32_t>(720 * main_scale)};
  const ::SDL_WindowFlags flags{SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN |
                                SDL_WINDOW_HIGH_PIXEL_DENSITY};
  ::SDL_Window *const window{::SDL_CreateWindow(title, window_w, window_h, flags)};
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

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (::gladLoadGL(reinterpret_cast<::GLADloadfunc>(::SDL_GL_GetProcAddress)) == 0) {
    spdlog::error("Failed to initialize GLAD");
    return EXIT_FAILURE;
  }
  spdlog::info("GLAD initialized successfully");

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("OpenGL version: {}", reinterpret_cast<const char *>(::glGetString(GL_VERSION)));
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("GLSL version: {}", reinterpret_cast<const char *>(::glGetString(GL_SHADING_LANGUAGE_VERSION)));
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("Vendor: {}", reinterpret_cast<const char *>(::glGetString(GL_VENDOR)));
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  spdlog::info("Renderer: {}", reinterpret_cast<const char *>(::glGetString(GL_RENDERER)));

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto imgui_cleanup{ScopeExit{[] -> void {
    ::ImGui_ImplOpenGL3_Shutdown();
    ::ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
  }}};
  ::ImGuiIO &io{ImGui::GetIO()};
  io.ConfigFlags |= ::ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ::ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ::ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ::ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  ::ImGuiStyle &style{ImGui::GetStyle()};
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

  bool done{false};

  while (!done) {
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
      default:
        break;
      }
    }

    if (static_cast<bool>(::SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) {
      ::SDL_Delay(10);
      continue;
    }

    ::ImGui_ImplOpenGL3_NewFrame();
    ::ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Render();
    const auto framebuffer_w{static_cast<std::int32_t>(std::round(io.DisplaySize.x * io.DisplayFramebufferScale.x))};
    const auto framebuffer_h{static_cast<std::int32_t>(std::round(io.DisplaySize.y * io.DisplayFramebufferScale.y))};
    ::glViewport(0, 0, framebuffer_w, framebuffer_h);
    ::glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    ::glClear(GL_COLOR_BUFFER_BIT);

    ::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (static_cast<bool>(io.ConfigFlags & ::ImGuiConfigFlags_ViewportsEnable)) {
      ::SDL_Window *const backup_current_window{::SDL_GL_GetCurrentWindow()};
      const ::SDL_GLContext backup_current_context{::SDL_GL_GetCurrentContext()};
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      ::SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

    ::SDL_GL_SwapWindow(window);
  }

  return EXIT_SUCCESS;
}
