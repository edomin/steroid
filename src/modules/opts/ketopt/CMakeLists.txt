if (${ST_MODULE_OPTS_KETOPT} STREQUAL "no")
    return()
endif()

set(ST_MODULE_NAME "ketopt")
set(ST_MODULE_TYPE ${ST_MODULE_OPTS_KETOPT})
set(ST_MODULE_SUBSYSTEM "opts")
set(ST_MODULE_TARGET st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME})

set(ST_MODULE_PREREQS [=[
    {
        .subsystem = "logger",
        .name = NULL,
    },
]=])
set(ST_MODULE_PREREQS_COUNT 1)

configure_file(
    "config.h.in"
    "config.h"
)

bb_add_compile_options(LANG C OPTIONS C_COMPILE_OPTIONS)
bb_add_more_warnings(
    LANG C
    CATEGORIES
        basic
        alloc
        array
        asciiz
        enum
        preprocessor
    OPTIONS C_COMPILE_OPTIONS
)

st_add_module(${ST_MODULE_TARGET} ${ST_MODULE_OPTS_KETOPT})
st_process_internal_module(${ST_MODULE_TARGET} ${ST_MODULE_TYPE})

find_package(PkgConfig REQUIRED)
pkg_check_modules(SAFEC REQUIRED libsafec)
find_library(LIBBSD_LIBRARY NAMES bsd)

target_compile_options(${ST_MODULE_TARGET} PRIVATE ${C_COMPILE_OPTIONS})
target_sources(${ST_MODULE_TARGET} PRIVATE
    "ketopt.c"
)

bb_set_c_std(${ST_MODULE_TARGET} STD 11 EXTENSIONS)

bb_add_iwyu_mapping_file(
    ${ST_MODULE_TARGET}
    ${CMAKE_CURRENT_SOURCE_DIR}/iwyu.imp
)

target_include_directories(${ST_MODULE_TARGET} PRIVATE
    "${CMAKE_FIND_ROOT_PATH}/include"
    "${CMAKE_SOURCE_DIR}/include"
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${ST_MODULE_TARGET} PRIVATE
    ${LIBBSD_LIBRARY}
    ${SAFEC_LINK_LIBRARIES}
)