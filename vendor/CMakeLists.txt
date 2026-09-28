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

add_subdirectory(assimp)
add_subdirectory(glad)
add_subdirectory(glm)
add_subdirectory(SDL)
add_subdirectory(spdlog)

add_library(
  imgui
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui.cpp
  imgui/backends/imgui_impl_sdl3.cpp
  imgui/backends/imgui_impl_opengl3.cpp)
add_library(imgui::imgui ALIAS imgui)
target_include_directories(
  imgui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
               $<INSTALL_INTERFACE:imgui>)
target_link_libraries(imgui PRIVATE SDL3::SDL3 ${CMAKE_DL_LIBS})

add_library(stb INTERFACE)
add_library(stb::stb ALIAS stb)
target_include_directories(
  stb INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb>
                $<INSTALL_INTERFACE:stb>)
