add_executable(example_002)
target_sources(example_002 PRIVATE main.cc)

target_link_libraries(example_002 PRIVATE glad::glad)
target_link_libraries(example_002 PRIVATE SDL3::SDL3)
target_link_libraries(example_002 PRIVATE spdlog::spdlog)
target_link_libraries(example_002 PRIVATE imgui::imgui)
