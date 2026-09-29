add_executable(example_004a)
target_sources(
  example_004a
  PRIVATE main.cc
          buffer.cc
          mesh.cc
          shader.cc
          program.cc
          camera.cc
          stb_image_impl.cc)

target_link_libraries(example_004a PRIVATE glad::glad)
target_link_libraries(example_004a PRIVATE glm::glm)
target_link_libraries(example_004a PRIVATE SDL3::SDL3)
target_link_libraries(example_004a PRIVATE spdlog::spdlog)
target_link_libraries(example_004a PRIVATE imgui::imgui)
target_link_libraries(example_004a PRIVATE stb::stb)

add_custom_command(
  TARGET example_004a
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:example_004a>/assets")
