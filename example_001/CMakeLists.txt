add_executable(example_001)
target_sources(example_001 PRIVATE main.cc)

target_link_libraries(example_001 PRIVATE glad::glad)
target_link_libraries(example_001 PRIVATE SDL3::SDL3)
target_link_libraries(example_001 PRIVATE spdlog::spdlog)
