if (${ST_MODULE_LOGGER_LIBSIR} STREQUAL "no")
    return()
endif()

set(ST_MODULE_NAME "libsir")
set(ST_MODULE_TYPE ${ST_MODULE_LOGGER_LIBSIR})
set(ST_MODULE_SUBSYSTEM "logger")
set(ST_MODULE_TARGET st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME})

set(ST_MODULE_PREREQS "")
set(ST_MODULE_PREREQS_COUNT 0)

configure_file(
    "config.h.in"
    "config.h"
)

bb_add_compile_options(LANG C OPTIONS C_COMPILE_OPTIONS)
bb_add_more_warnings(
    LANG C
    CATEGORIES
        basic
        array
        asciiz
        enum
        format
        preprocessor
    OPTIONS C_COMPILE_OPTIONS
)

st_add_module(${ST_MODULE_TARGET} ${ST_MODULE_OPTS_KETOPT})
st_process_internal_module(${ST_MODULE_TARGET} ${ST_MODULE_TYPE})

find_library(LIBSIR_LIBRARY NAMES sir_s)
find_package(Threads)

target_compile_options(${ST_MODULE_TARGET} PRIVATE ${C_COMPILE_OPTIONS})
target_sources(${ST_MODULE_TARGET} PRIVATE
    "libsir.c"
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
    ${LIBSIR_LIBRARY}
    Threads::Threads
)
