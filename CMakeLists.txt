cmake_minimum_required(VERSION 3.7.2)
cmake_policy(SET CMP0069 NEW)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE
        STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

project(steroids LANGUAGES C)

set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    "${CMAKE_SOURCE_DIR}/cmake/modules"
    "${CMAKE_SOURCE_DIR}/cmake/packages"
    "${CMAKE_SOURCE_DIR}/cmake/toolchains"
)
include(options)

include(CheckCCompilerFlag)
include(CheckIPOSupported OPTIONAL RESULT_VARIABLE CHECK_IPO_SUPPORTED_PATH)
if (CHECK_IPO_SUPPORTED_PATH)
    check_ipo_supported(RESULT ST_HAVE_LTO)
else()
    check_c_compiler_flag("-flto" ST_HAVE_FLTO)
endif()

include(CheckIncludeFile)
include(CheckSymbolExists)
check_include_file(sys/queue.h ST_HAVE_QUEUE_H)
check_symbol_exists(strlcpy string.h ST_HAVE_STRLCPY)
configure_file (
    "${CMAKE_SOURCE_DIR}/include/steroids/config.h.in"
    "${CMAKE_BINARY_DIR}/include/steroids/config.h"
)

file(WRITE "${CMAKE_BINARY_DIR}/dummy.c" "")
set(ST_INTERNAL_MODULES_COUNT 0)
include(utils)
include(build-common)
include(build-module)
include(modules)
include(build-logger-simple)
include(build-logger-libsir)

foreach(ST_INTERNAL_MODULE ${ST_INTERNAL_MODULES})
    set(INTERNAL_MODULES_INIT_FUNCS_PROTOTYPES
     "${INTERNAL_MODULES_INIT_FUNCS_PROTOTYPES}\
     extern st_moddata_t *st_module_${ST_INTERNAL_MODULE}_init(\
      void *, st_modsmgr_get_function_t);\n"
    )
    set(ST_INTERNAL_MODULES_INIT_FUNCS
     "${ST_INTERNAL_MODULES_INIT_FUNCS}\
     st_module_${ST_INTERNAL_MODULE}_init,\n        ")
endforeach(ST_INTERNAL_MODULE)
configure_file (
    "${CMAKE_SOURCE_DIR}/src/core/internal_modules.h.in"
    "${CMAKE_BINARY_DIR}/src/core/internal_modules.h"
)

include(build-core)
