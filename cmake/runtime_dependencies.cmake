function(copy_runtime_dependencies target)
    if(NOT BUILD_STATIC AND NOT UNIX)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${target}>
            $<TARGET_FILE_DIR:${target}>
            COMMAND_EXPAND_LISTS
    )
    endif()
endfunction()

function(copy_executable src_target target)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${src_target}>
            $<TARGET_FILE_DIR:${target}>
    )
endfunction()