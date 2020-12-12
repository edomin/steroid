# By Vasiliy Edomin
# Based on FindFFMPEG.cmake
# Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#
# Defines:
#  LIST_FOUND
#  LIST_INCLUDE_DIR
#  LIST_LIBRARY

if (LIST_LIBRARY AND LIST_INCLUDE_DIR)
    # in cache already
    set(LIST_FOUND TRUE)
else ()
    find_path(LIST_INCLUDE_DIR
        NAMES list.h
    )

    find_library(LIST_LIBRARY
        NAMES list
    )

    if (LIST_LIBRARY AND LIST_INCLUDE_DIR)
        set(LIST_FOUND TRUE)
    endif()

    if (LIST_FOUND)
        if (NOT LIST_FIND_QUIETLY)
            message(STATUS "Found list: ${LIST_LIBRARY}")
        endif ()
    else ()
        if (LIST_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find list")
        endif ()
    endif ()
endif ()

