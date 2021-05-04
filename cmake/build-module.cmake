function(st_target_set_module_common ST_TARGET ST_BUILD_MODE)
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

    add_library(${ST_TARGET} ${ST_LIBRARY_MODE} "${CMAKE_BINARY_DIR}/dummy.c")

    st_target_set_common(${ST_TARGET})
endfunction()

macro(st_process_internal_module ST_TARGET ST_MODULE_TYPE)
    if (${ST_MODULE_TYPE} STREQUAL "internal")
        math(EXPR ST_INTERNAL_MODULES_COUNT "${ST_INTERNAL_MODULES_COUNT} + 1")
        set(ST_MODULE_FULL_NAME
            "${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}"
        )
        list(APPEND ST_INTERNAL_MODULES ${ST_MODULE_FULL_NAME})
        list(APPEND ST_INTERNAL_MODULES_LIBS ${ST_TARGET})
    endif()
endmacro()
