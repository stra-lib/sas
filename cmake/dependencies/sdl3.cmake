if(NOT TARGET sdl3)
    include(FetchContent)

    if (BUILD_STATIC)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
    endif ()

    if (UNIX)
        set(SDL_UNIX_CONSOLE_BUILD ON CACHE BOOL "" FORCE)
    endif ()

    FetchContent_Declare(
            sdl3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.4.x
    )
    FetchContent_MakeAvailable(sdl3)

    add_library(sdl3 INTERFACE)
    target_link_libraries(sdl3 INTERFACE SDL3::SDL3)
endif ()
