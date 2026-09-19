add_executable(example_202)
target_sources(example_202 PRIVATE src/main.cpp src/shader.cpp src/camera.cpp
                                   src/stb_image_impl.cpp)
target_include_directories(example_202 PRIVATE src)

find_program(CLANG_TIDY_PROGRAM NAMES clang-tidy)
if(CLANG_TIDY_PROGRAM AND NOT ENABLE_TIME_TRACE)
  set_target_properties(example_202 PROPERTIES CXX_CLANG_TIDY
                                               "${CLANG_TIDY_PROGRAM}")
endif()

if(MSVC)
  target_compile_options(example_202 PRIVATE /W4)
else()
  target_compile_options(example_202 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# target_link_libraries(example_202 PRIVATE assimp::assimp)
# target_link_libraries(example_202 PRIVATE box3d::box3d)
target_link_libraries(example_202 PRIVATE glad::glad)
target_link_libraries(example_202 PRIVATE glm::glm)
target_link_libraries(example_202 PRIVATE SDL3::SDL3)
target_link_libraries(example_202 PRIVATE spdlog::spdlog)
target_link_libraries(example_202 PRIVATE imgui::imgui)
target_link_libraries(example_202 PRIVATE stb::stb)

target_compile_definitions(
  example_202 PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
