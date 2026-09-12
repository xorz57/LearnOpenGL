add_executable(example_101)
target_sources(example_101 PRIVATE src/main.cpp)
target_include_directories(example_101 PRIVATE src)
find_program(CLANG_TIDY_PROGRAM NAMES clang-tidy)
if(CLANG_TIDY_PROGRAM)
  set_target_properties(example_101 PROPERTIES CXX_CLANG_TIDY
                                               "${CLANG_TIDY_PROGRAM}")
endif()
if(MSVC)
  target_compile_options(example_101 PRIVATE /W4)
else()
  target_compile_options(example_101 PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_link_libraries(
  example_101
  PRIVATE assimp::assimp
          glad::glad
          glm::glm
          SDL3::SDL3
          spdlog::spdlog
          imgui::imgui
          stb::image)
