if (${ST_MODULE_LOGGER_SIMPLE} STREQUAL "internal")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()
find_package(libsir REQUIRED)

set(ST_MODULE_NAME "libsir")
set(ST_MODULE_TYPE ${ST_MODULE_LOGGER_SIMPLE})
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

st_target_set_module_common(st_logger_libsir ${ST_MODULE_LOGGER_LIBSIR})
st_process_internal_module(st_logger_libsir ST_MODULE_TYPE)

target_include_directories(st_logger_libsir PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/modules/logger/libsir/>
    ${LIBSIR_INCLUDE_DIR}
)
target_link_libraries(st_logger_libsir PRIVATE
    ${LIBSIR_LIBRARY}
    Threads::Threads
)
target_sources(st_logger_libsir PRIVATE
    "${CMAKE_SOURCE_DIR}/src/modules/logger/libsir/libsir.c"
)
