#include "scope_exit.h"

#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <cstdint>
#include <cstdlib>

auto main() -> int {
  if (!::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    spdlog::error("Failed to initialize SDL: {}", ::SDL_GetError());
    return EXIT_FAILURE;
  }
  spdlog::info("SDL initialized successfully");
  auto sdl_cleanup{ScopeExit{[] -> void { ::SDL_Quit(); }}};

  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_FLAGS, 0);
  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  ::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_MINOR_VERSION, 6);

  ::SDL_GL_SetAttribute(::SDL_GL_DOUBLEBUFFER, 1);
  ::SDL_GL_SetAttribute(::SDL_GL_DEPTH_SIZE, 24);
  ::SDL_GL_SetAttribute(::SDL_GL_STENCIL_SIZE, 8);

  const float main_scale{::SDL_GetDisplayContentScale(::SDL_GetPrimaryDisplay())};

  const char* title{"example_001"};
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

  bool done{false};

  while (!done) {
    ::SDL_Event event{};
    while (::SDL_PollEvent(&event)) {
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

    std::int32_t framebuffer_w{};
    std::int32_t framebuffer_h{};
    ::SDL_GetWindowSizeInPixels(window, &framebuffer_w, &framebuffer_h);
    ::glViewport(0, 0, framebuffer_w, framebuffer_h);
    ::glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    ::glClear(GL_COLOR_BUFFER_BIT);

    ::SDL_GL_SwapWindow(window);
  }

  return EXIT_SUCCESS;
}
