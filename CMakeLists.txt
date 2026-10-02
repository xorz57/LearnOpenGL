cmake_minimum_required(VERSION 3.30)
project(LearnOpenGL VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(vendor SYSTEM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(ClangTidy)

add_subdirectory(example_001)
add_subdirectory(example_002)
add_subdirectory(example_003a)
add_subdirectory(example_003b)
add_subdirectory(example_004a)
add_subdirectory(example_004b)
add_subdirectory(example_005a)
add_subdirectory(example_005b)
