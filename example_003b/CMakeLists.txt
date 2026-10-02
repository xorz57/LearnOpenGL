add_executable(example_003b)
target_sources(example_003b PRIVATE main.cc shader.cc program.cc camera.cc
                                    stb_image_impl.cc)

target_link_libraries(example_003b PRIVATE glad::glad)
target_link_libraries(example_003b PRIVATE glm::glm)
target_link_libraries(example_003b PRIVATE SDL3::SDL3)
target_link_libraries(example_003b PRIVATE spdlog::spdlog)
target_link_libraries(example_003b PRIVATE imgui::imgui)
target_link_libraries(example_003b PRIVATE stb::stb)

add_custom_command(
  TARGET example_003b
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:example_003b>/assets")
