if(NOT TARGET asio)
    include(FetchContent)
    
    FetchContent_Declare(
            asio
            GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(asio)

    add_library(asio INTERFACE)

    target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
    target_compile_features(asio INTERFACE cxx_std_23)
endif()
