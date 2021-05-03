# Defines:
#  LIST_FOUND
#  LIST_INCLUDE_DIR
#  LIST_LIBRARY

if (LIST_LIBRARY AND LIST_INCLUDE_DIR)
    set(LIST_FOUND TRUE)
else ()
    message(STATUS "Looking for list.h")
    find_path(LIST_INCLUDE_DIR
        NAMES list.h
    )
    if (LIST_INCLUDE_DIR)
        message(STATUS "Looking for list.h - ${LIST_INCLUDE_DIR}/list.h")
    else()
        message(STATUS "Looking for list.h - not found")
    endif()

    message(STATUS "Looking for liblist.a")
    find_library(LIST_LIBRARY
        NAMES liblist.a list
    )
    if (LIST_LIBRARY)
        message(STATUS "Looking for liblist.a - ${LIST_LIBRARY}")
    else()
        message(STATUS "Looking for liblist.a - not found")
    endif()

    if (LIST_LIBRARY AND LIST_INCLUDE_DIR)
        set(LIST_FOUND TRUE)
    else()
        message(FATAL_ERROR "Could not find list")
    endif()
endif ()

