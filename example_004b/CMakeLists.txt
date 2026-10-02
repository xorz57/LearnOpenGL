add_executable(example_004b)
target_sources(example_004b PRIVATE main.cc mesh.cc shader.cc program.cc
                                    camera.cc stb_image_impl.cc)

target_link_libraries(example_004b PRIVATE glad::glad)
target_link_libraries(example_004b PRIVATE glm::glm)
target_link_libraries(example_004b PRIVATE SDL3::SDL3)
target_link_libraries(example_004b PRIVATE spdlog::spdlog)
target_link_libraries(example_004b PRIVATE imgui::imgui)
target_link_libraries(example_004b PRIVATE stb::stb)

add_custom_command(
  TARGET example_004b
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:example_004b>/assets")
