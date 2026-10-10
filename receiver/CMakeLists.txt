cmake_minimum_required(VERSION 4.2)
project(receiver)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(runtime_dependencies)

include(dependencies/asio)
include(dependencies/cereal)
include(dependencies/sdl3)

add_executable(receiver
	src/main.cpp
)

target_link_libraries(receiver
        asio
        cereal
        sdl3
        common
)

copy_runtime_dependencies(receiver)

target_compile_definitions(receiver PRIVATE
        PROJECT_NAME="SasReceiver"
        PROJECT_VERSION="${CMAKE_PROJECT_VERSION}"
        PROJECT_IDENTIFIER="${PROJECT_IDENTIFIER}"
)

set_target_properties(receiver PROPERTIES OUTPUT_NAME "SasReceiver")
