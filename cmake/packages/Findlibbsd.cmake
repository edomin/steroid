# Defines:
#  LIBBSD_FOUND
#  LIBBSD_INCLUDE_DIR
#  LIBBSD_LIBRARY

if (LIBBSD_LIBRARY AND LIBBSD_INCLUDE_DIR)
    set(LIBBSD_FOUND TRUE)
else ()
    include(CheckSymbolExists)
    check_symbol_exists(BSD sys/param.h ST_HAVE_BSD)
    if (${ST_HAVE_BSD})
        return()
    endif()

    message(STATUS "Looking for bsd/sys/freebsd-cdefs.h")
    find_path(LIBBSD_INCLUDE_DIR NAMES bsd/sys/freebsd-cdefs.h)
    if (LIBBSD_INCLUDE_DIR)
        message(STATUS "Looking for bsd/sys/freebsd-cdefs.h - ${LIBBSD_INCLUDE_DIR}/bsd/sys/freebsd-cdefs.h")
    else()
        message(STATUS "Looking for bsd/sys/freebsd-cdefs.h - not found")
    endif()

    message(STATUS "Looking for libbsd.a")
    find_library(LIBBSD_LIBRARY
        NAMES libbsd.a libbsd
    )
    if (LIBBSD_LIBRARY)
        message(STATUS "Looking for libbsd.a - ${LIBBSD_LIBRARY}")
    else()
        message(STATUS "Looking for libbsd.a - not found")
    endif()

    if (LIBBSD_LIBRARY AND LIBBSD_INCLUDE_DIR)
        set(LIBBSD_FOUND TRUE)
    else()
        message(FATAL_ERROR "Could not find libbsd")
    endif()
endif ()

