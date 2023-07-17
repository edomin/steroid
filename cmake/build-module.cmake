function(st_add_module ST_TARGET ST_BUILD_MODE)
    if (ST_BUILD_MODE STREQUAL "no")
        return()
    elseif (ST_BUILD_MODE STREQUAL "internal")
        set(ST_LIBRARY_MODE STATIC)
    elseif (ST_BUILD_MODE STREQUAL "shared")
        set(ST_LIBRARY_MODE SHARED)
    else()
        message(WARNING
            "Incorrect build mode for module ${ST_TARGET}. Module will not be \
            builded"
        )
        return()
    endif()

    bb_add_library(${ST_TARGET} ${ST_LIBRARY_MODE})

    if (ST_BUILD_MODE STREQUAL "shared")
        add_custom_target(${ST_TARGET}.stp
            ALL
            COMMAND
                ${CMAKE_COMMAND} -E make_directory ${BB_TRIPLET}
            COMMAND
                ${CMAKE_COMMAND} -E copy lib${ST_TARGET}.so ${BB_TRIPLET}
            COMMAND
                ${CMAKE_COMMAND} -E tar cfv
                ${ST_TARGET}.stp --format=zip ${BB_TRIPLET}/lib${ST_TARGET}.so
            COMMAND
                ${CMAKE_COMMAND} -E copy ${ST_TARGET}.stp ${CMAKE_BINARY_DIR}
            DEPENDS
                ${ST_TARGET}
        )

        add_dependencies(copy_plugins ${ST_TARGET}.stp)
    endif()
endfunction()

macro(st_process_internal_module ST_TARGET ST_MODULE_TYPE)
    if (${ST_MODULE_TYPE} STREQUAL "internal")
        math(EXPR ST_INTERNAL_MODULES_COUNT "${ST_INTERNAL_MODULES_COUNT} + 1")
        set(ST_MODULE_FULL_NAME
            "${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}"
        )
        list(APPEND ST_INTERNAL_MODULES ${ST_MODULE_FULL_NAME})
        list(APPEND ST_INTERNAL_MODULES_LIBS ${ST_TARGET})

        set(ST_INTERNAL_MODULES ${ST_INTERNAL_MODULES} PARENT_SCOPE)
        set(ST_INTERNAL_MODULES_LIBS ${ST_INTERNAL_MODULES_LIBS} PARENT_SCOPE)
        set(ST_INTERNAL_MODULES_COUNT ${ST_INTERNAL_MODULES_COUNT} PARENT_SCOPE)
    endif()
endmacro()
