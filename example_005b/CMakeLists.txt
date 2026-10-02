add_executable(example_005b)
target_sources(
  example_005b
  PRIVATE main.cc
          buffer.cc
          mesh.cc
          shader.cc
          program.cc
          camera.cc
          stb_image_impl.cc)

target_link_libraries(example_005b PRIVATE glad::glad)
target_link_libraries(example_005b PRIVATE glm::glm)
target_link_libraries(example_005b PRIVATE SDL3::SDL3)
target_link_libraries(example_005b PRIVATE spdlog::spdlog)
target_link_libraries(example_005b PRIVATE imgui::imgui)
target_link_libraries(example_005b PRIVATE stb::stb)

add_custom_command(
  TARGET example_005b
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:example_005b>/assets")
