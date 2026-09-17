cmake_minimum_required(VERSION 3.20...3.30)

project(LearnOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF)

set(ASSIMP_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS
    OFF
    CACHE BOOL "" FORCE)

set(SDL_TEST_LIBRARY
    OFF
    CACHE BOOL "" FORCE)

add_subdirectory(vendor/assimp EXCLUDE_FROM_ALL)
add_subdirectory(vendor/box3d EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor/spdlog EXCLUDE_FROM_ALL)

add_library(imgui STATIC EXCLUDE_FROM_ALL )
add_library(imgui::imgui ALIAS imgui)
target_sources(
  imgui
  PRIVATE vendor/imgui/imgui_demo.cpp
          vendor/imgui/imgui_draw.cpp
          vendor/imgui/imgui_tables.cpp
          vendor/imgui/imgui_widgets.cpp
          vendor/imgui/imgui.cpp
          vendor/imgui/backends/imgui_impl_sdl3.cpp
          vendor/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC vendor vendor/imgui)
target_link_libraries(imgui PRIVATE SDL3::SDL3 ${CMAKE_DL_LIBS})

add_library(stb INTERFACE)
add_library(stb::stb ALIAS stb)
target_include_directories(stb INTERFACE vendor)

add_subdirectory(example_001)
add_subdirectory(example_101)
add_subdirectory(example_102)
