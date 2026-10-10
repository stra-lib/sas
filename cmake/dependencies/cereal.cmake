if(NOT TARGET cereal)
    include(FetchContent)

    set(BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
    set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
            cereal
            GIT_REPOSITORY https://github.com/USCiLab/cereal.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(cereal)
endif ()