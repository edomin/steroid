find_package(libsir REQUIRED)

set(ST_MODULE_NAME "libsir")
set(ST_MODULE_TYPE ${ST_MODULE_LOGGER_LIBSIR})
set(ST_MODULE_SUBSYSTEM "logger")

set(ST_MODULE_PREREQS [=[
    {
        .subsystem = "logger",
        .name = "simple",
    },
]=])
set(ST_MODULE_PREREQS_COUNT 1)

configure_file(
    "${CMAKE_SOURCE_DIR}/src/modules/logger/libsir/config.h.in"
    "${CMAKE_BINARY_DIR}/src/modules/logger/libsir/config.h"
)

st_target_set_module_common(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}
    ${ST_MODULE_LOGGER_LIBSIR})
st_process_internal_module(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}
    ${ST_MODULE_TYPE})

target_include_directories(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/modules/logger/libsir/>
    ${LIBSIR_INCLUDE_DIR}
)
target_link_libraries(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    ${LIBSIR_LIBRARY}
)
target_sources(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/modules/logger/libsir/libsir.c"
)
