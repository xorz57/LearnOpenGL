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

target_link_libraries(example_001 PRIVATE assimp::assimp)
target_link_libraries(example_001 PRIVATE box3d::box3d)
target_link_libraries(example_001 PRIVATE glad::glad)
target_link_libraries(example_001 PRIVATE glm::glm)
target_link_libraries(example_001 PRIVATE SDL3::SDL3)
target_link_libraries(example_001 PRIVATE spdlog::spdlog)
target_link_libraries(example_001 PRIVATE imgui::imgui)
target_link_libraries(example_001 PRIVATE stb::image)
