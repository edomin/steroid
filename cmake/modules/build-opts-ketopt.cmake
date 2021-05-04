find_package(ketopt REQUIRED)

set(ST_MODULE_NAME "ketopt")
set(ST_MODULE_TYPE ${ST_MODULE_OPTS_KETOPT})
set(ST_MODULE_SUBSYSTEM "opts")

set(ST_MODULE_PREREQS [=[
    {
        .subsystem = "logger",
        .name = NULL,
    },
]=])
set(ST_MODULE_PREREQS_COUNT 1)

configure_file(
    "${CMAKE_SOURCE_DIR}/src/modules/opts/ketopt/config.h.in"
    "${CMAKE_BINARY_DIR}/src/modules/opts/ketopt/config.h"
)

st_target_set_module_common(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}
    ${ST_MODULE_OPTS_KETOPT})
st_process_internal_module(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME}
    ${ST_MODULE_TYPE})

target_include_directories(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/modules/opts/ketopt/>
    ${KETOPT_INCLUDE_DIR}
)
target_link_libraries(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    ${KETOPT_LIBRARY}
)
if (NOT ST_HAVE_STRLCPY)
    find_package(libbsd REQUIRED)
    target_link_libraries(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
        ${LIBBSD_LIBRARY}
    )
endif()
target_sources(st_${ST_MODULE_SUBSYSTEM}_${ST_MODULE_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/modules/opts/ketopt/ketopt.c"
)
