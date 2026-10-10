cmake_minimum_required(VERSION 4.2)
project(sender)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(runtime_dependencies)

include(dependencies/asio)
include(dependencies/cereal)
include(dependencies/sdl3)

add_executable(sender
        src/main.cpp
        src/sender.cpp
        src/sender.hpp
)

target_link_libraries(sender
        asio
        cereal
        sdl3
        common
)

copy_runtime_dependencies(sender)

target_compile_definitions(sender PRIVATE
        PROJECT_NAME="SasSender"
        PROJECT_VERSION="${CMAKE_PROJECT_VERSION}"
        PROJECT_IDENTIFIER="${PROJECT_IDENTIFIER}"
)

set_target_properties(sender PROPERTIES OUTPUT_NAME "SasSender")
