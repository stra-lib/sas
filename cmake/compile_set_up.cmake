if(MSVC)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

if(BUILD_STATIC)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    if(UNIX)
        add_link_options(-static-libgcc -static-libstdc++)
    endif()

endif()