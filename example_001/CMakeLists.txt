add_executable(example_001)
target_sources(example_001 PRIVATE src/main.cpp)
target_include_directories(example_001 PRIVATE src)
find_program(CLANG_TIDY_PROGRAM NAMES clang-tidy)
if(CLANG_TIDY_PROGRAM)
  set_target_properties(example_001 PROPERTIES CXX_CLANG_TIDY
                                               "${CLANG_TIDY_PROGRAM}")
endif()
if(MSVC)
  target_compile_options(example_001 PRIVATE /W4)
else()
  target_compile_options(example_001 PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_link_libraries(
  example_001
  PRIVATE assimp::assimp
          box3d::box3d
          glad::glad
          glm::glm
          SDL3::SDL3
          spdlog::spdlog
          imgui::imgui
          stb::image)
