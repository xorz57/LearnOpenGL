add_executable(example_102)
target_sources(example_102 PRIVATE src/main.cpp src/shader.cpp src/camera.cpp)
target_include_directories(example_102 PRIVATE src)
find_program(CLANG_TIDY_PROGRAM NAMES clang-tidy)
if(CLANG_TIDY_PROGRAM)
  set_target_properties(example_102 PROPERTIES CXX_CLANG_TIDY
                                               "${CLANG_TIDY_PROGRAM}")
endif()
if(MSVC)
  target_compile_options(example_102 PRIVATE /W4)
else()
  target_compile_options(example_102 PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_link_libraries(
  example_102
  PRIVATE assimp::assimp
          glad::glad
          glm::glm
          SDL3::SDL3
          spdlog::spdlog
          imgui::imgui
          stb::image)
target_compile_definitions(
  example_102 PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
