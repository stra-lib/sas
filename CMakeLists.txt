cmake_minimum_required(VERSION 4.2)
project(sas VERSION 2026.919.0)

set(PROJECT_IDENTIFIER "io.github.stra-lib.sas")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_STATIC "Build with static dependencies" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(compile_set_up)

add_subdirectory(common)
add_subdirectory(receiver)
add_subdirectory(sender)
