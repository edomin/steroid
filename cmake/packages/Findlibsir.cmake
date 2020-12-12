# By Vasiliy Edomin
# Based on FindFFMPEG.cmake
# Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#
# Defines:
#  LIBSIR_FOUND
#  LIBSIR_INCLUDE_DIR
#  LIBSIR_LIBRARY

if (LIBSIR_LIBRARY AND LIBSIR_INCLUDE_DIR)
    # in cache already
    set(LIBSIR_FOUND TRUE)
else ()
    find_path(LIBSIR_INCLUDE_DIR
        NAMES sir.h
    )

    find_library(LIBSIR_LIBRARY
        NAMES sir_s
    )

    if (LIBSIR_LIBRARY AND LIBSIR_INCLUDE_DIR)
        set(LIBSIR_FOUND TRUE)
    endif()

    if (LIBSIR_FOUND)
        if (NOT LIBSIR_FIND_QUIETLY)
            message(STATUS "Found libsir: ${LIBSIR_LIBRARY}")
        endif ()
    else ()
        if (LIBSIR_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find libsir")
        endif ()
    endif ()
endif ()

