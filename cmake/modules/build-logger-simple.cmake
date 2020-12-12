set(ST_MODULE_NAME "simple")
string(LENGTH ${ST_MODULE_NAME} ST_MODULE_NAME_LEN)
set(ST_MODULE_TYPE ${ST_MODULE_LOGGER_SIMPLE})
string(LENGTH ${ST_MODULE_TYPE} ST_MODULE_TYPE_LEN)
set(ST_MODULE_SUBSYSTEM "logger")
string(LENGTH ${ST_MODULE_SUBSYSTEM} ST_MODULE_SUBSYSTEM_LEN)
set(ST_MODULE_PREREQS
    ""
)
set(ST_MODULE_PREREQS_COUNT 0)

configure_file(
    "${CMAKE_SOURCE_DIR}/src/modules/logger/simple/config.h.in"
    "${CMAKE_BINARY_DIR}/src/modules/logger/simple/config.h"
)

st_target_set_module_common(st_logger_simple ${ST_MODULE_LOGGER_SIMPLE})
st_process_internal_module(st_logger_simple ST_MODULE_TYPE)

target_include_directories(st_logger_simple PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/modules/logger/simple/>
)
target_sources(st_logger_simple PRIVATE
    "${CMAKE_SOURCE_DIR}/src/modules/logger/simple/simple.c"
)
