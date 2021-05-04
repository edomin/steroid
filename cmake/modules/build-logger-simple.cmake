set(ST_MODULE_NAME "simple")
set(ST_MODULE_TYPE ${ST_MODULE_LOGGER_SIMPLE})
set(ST_MODULE_SUBSYSTEM "logger")

set(ST_MODULE_PREREQS
    ""
)
set(ST_MODULE_PREREQS_COUNT 0)

configure_file(
    "${CMAKE_SOURCE_DIR}/src/modules/logger/simple/config.h.in"
    "${CMAKE_BINARY_DIR}/src/modules/logger/simple/config.h"
)

st_target_set_module_common(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}
    ${ST_MODULE_LOGGER_SIMPLE})
st_process_internal_module(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}
    ${ST_MODULE_TYPE})

target_include_directories(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/modules/logger/simple/>
)

target_sources(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/modules/logger/simple/simple.c"
)
