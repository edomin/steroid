cmake_minimum_required(VERSION 3.9)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${BB_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(barebones/barebones)
include(options)
include(build-module)

project(steroids LANGUAGES C)

set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    "${CMAKE_SOURCE_DIR}/cmake/modules"
)

set(ST_INTERNAL_MODULES_COUNT 0)
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/spcpaths/cfgpath"
 "${CMAKE_BINARY_DIR}/src/modules/spcpaths/cfgpath")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/fnv1a/fnv_hash"
 "${CMAKE_BINARY_DIR}/src/modules/fnv1a/fnv_hash")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/fs/simple"
 "${CMAKE_BINARY_DIR}/src/modules/fs/simple")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/hash_table/hash_table"
 "${CMAKE_BINARY_DIR}/src/modules/hash_table/hash_table")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/ini/inih"
 "${CMAKE_BINARY_DIR}/src/modules/ini/inih")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/logger/simple"
 "${CMAKE_BINARY_DIR}/src/modules/logger/simple")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/logger/libsir"
 "${CMAKE_BINARY_DIR}/src/modules/logger/libsir")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/opts/ketopt"
 "${CMAKE_BINARY_DIR}/src/modules/opts/ketopt")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/pathtools/simple"
 "${CMAKE_BINARY_DIR}/src/modules/pathtools/simple")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/plugin/simple"
 "${CMAKE_BINARY_DIR}/src/modules/plugin/simple")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/so/simple"
 "${CMAKE_BINARY_DIR}/src/modules/so/simple")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/modules/zip/zip"
 "${CMAKE_BINARY_DIR}/src/modules/zip/zip")

foreach(ST_INTERNAL_MODULE ${ST_INTERNAL_MODULES})
    set(INTERNAL_MODULES_INIT_FUNCS_PROTOTYPES
     "${INTERNAL_MODULES_INIT_FUNCS_PROTOTYPES}\
     extern st_moddata_t *st_module_${ST_INTERNAL_MODULE}_init(\
      st_modsmgr_t *, st_modsmgr_funcs_t *);\n"
    )
    set(ST_INTERNAL_MODULES_INIT_FUNCS
     "${ST_INTERNAL_MODULES_INIT_FUNCS}\
     st_module_${ST_INTERNAL_MODULE}_init,\n        ")
endforeach(ST_INTERNAL_MODULE)
configure_file (
    "${CMAKE_SOURCE_DIR}/src/core/internal_modules.h.in"
    "${CMAKE_BINARY_DIR}/src/core/internal_modules.h"
)

add_subdirectory("${CMAKE_SOURCE_DIR}/src/core" "${CMAKE_BINARY_DIR}/src/core")
