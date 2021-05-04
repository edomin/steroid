# Defines:
#  KETOPT_FOUND
#  KETOPT_INCLUDE_DIR
#  KETOPT_LIBRARY

set(KETOPT_LIBRARY "" CACHE INTERNAL "")

if (KETOPT_INCLUDE_DIR)
    set(KETOPT_FOUND TRUE)
else ()
    message(STATUS "Looking for ketopt.h")
    find_path(KETOPT_INCLUDE_DIR
        NAMES ketopt.h
    )
    if (KETOPT_INCLUDE_DIR)
        message(STATUS "Looking for ketopt.h - ${KETOPT_INCLUDE_DIR}/ketopt.h")
    else()
        message(STATUS "Looking for ketopt.h - not found")
    endif()

    if (KETOPT_INCLUDE_DIR)
        set(KETOPT_FOUND TRUE)
    else()
        message(FATAL_ERROR "Could not find ketopt")
    endif()
endif ()

