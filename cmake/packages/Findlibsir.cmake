# Defines:
#  LIBSIR_FOUND
#  LIBSIR_INCLUDE_DIR
#  LIBSIR_LIBRARY

if (LIBSIR_LIBRARY AND LIBSIR_INCLUDE_DIR)
    set(LIBSIR_FOUND TRUE)
else ()
    message(STATUS "Looking for sir.h")
    find_path(LIBSIR_INCLUDE_DIR
        NAMES sir.h
    )
    if (LIBSIR_INCLUDE_DIR)
        message(STATUS "Looking for sir.h - ${LIBSIR_INCLUDE_DIR}/sir.h")
    else()
        message(STATUS "Looking for sir.h - not found")
    endif()

    message(STATUS "Looking for libsir_s.a")
    find_library(LIBSIR_LIBRARY
        NAMES sir_s
    )
    if (LIBSIR_LIBRARY)
        message(STATUS "Looking for libsir_s.a - ${LIBSIR_LIBRARY}")
    else()
        message(STATUS "Looking for libsir_s.a - not found")
    endif()

    if (LIBSIR_LIBRARY AND LIBSIR_INCLUDE_DIR)
        set(LIBSIR_FOUND TRUE)
    else()
        message(FATAL_ERROR "Could not find libsir")
    endif()
endif ()
