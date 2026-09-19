add_executable(example_103)
target_sources(example_103 PRIVATE src/main.cpp src/shader.cpp src/camera.cpp
                                   src/stb_image_impl.cpp)
target_include_directories(example_103 PRIVATE src)

find_program(CLANG_TIDY_PROGRAM NAMES clang-tidy)
if(CLANG_TIDY_PROGRAM AND NOT ENABLE_TIME_TRACE)
  set_target_properties(example_103 PROPERTIES CXX_CLANG_TIDY
                                               "${CLANG_TIDY_PROGRAM}")
endif()

if(MSVC)
  target_compile_options(example_103 PRIVATE /W4)
else()
  target_compile_options(example_103 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# target_link_libraries(example_103 PRIVATE assimp::assimp)
# target_link_libraries(example_103 PRIVATE box3d::box3d)
target_link_libraries(example_103 PRIVATE glad::glad)
target_link_libraries(example_103 PRIVATE glm::glm)
target_link_libraries(example_103 PRIVATE SDL3::SDL3)
target_link_libraries(example_103 PRIVATE spdlog::spdlog)
target_link_libraries(example_103 PRIVATE imgui::imgui)
target_link_libraries(example_103 PRIVATE stb::stb)

target_compile_definitions(
  example_103 PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
