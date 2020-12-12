# By Vasiliy Edomin
# Based on FindFFMPEG.cmake
# Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#
# Defines:
#  TINYFILEDIALOGS_FOUND
#  TINYFILEDIALOGS_INCLUDE_DIR
#  TINYFILEDIALOGS_LIBRARY

if (TINYFILEDIALOGS_LIBRARY AND TINYFILEDIALOGS_INCLUDE_DIR)
    # in cache already
    set(TINYFILEDIALOGS_FOUND TRUE)
else ()
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        pkg_check_modules(_TINYFILEDIALOGS tinyfiledialogs)
    endif ()

    find_path(TINYFILEDIALOGS_INCLUDE_DIR
        NAMES tinyfiledialogs.h
        #PATHS ${_TINYFILEDIALOGS_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include /sw/include
    )

    find_library(TINYFILEDIALOGS_LIBRARY
        NAMES tinyfiledialogs
        #PATHS ${_TINYFILEDIALOGS_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib
    )

    if (TINYFILEDIALOGS_LIBRARY AND TINYFILEDIALOGS_INCLUDE_DIR)
        set(TINYFILEDIALOGS_FOUND TRUE)
    endif()

    if (TINYFILEDIALOGS_FOUND)
        if (NOT TINYFILEDIALOGS_FIND_QUIETLY)
            message(STATUS "Found tinyfiledialogs: ${TINYFILEDIALOGS_LIBRARY}")
        endif ()
    else ()
        if (TINYFILEDIALOGS_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find tinyfiledialogs")
        endif ()
    endif ()
endif ()

