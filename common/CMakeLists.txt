cmake_minimum_required(VERSION 4.2)

add_library(common STATIC
        src/animation.cpp
        src/helper.cpp
        src/sdl_guard.cpp
        src/audio_stream.cpp
        src/audio_stream_debugger.cpp
)

target_include_directories(common PUBLIC
	include/
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(dependencies/asio)
include(dependencies/sdl3)

target_link_libraries(common
        asio
        sdl3
)
