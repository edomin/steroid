bb_add_compile_options(LANG C OPTIONS C_COMPILE_OPTIONS)
bb_add_more_warnings(
    LANG C
    CATEGORIES
        basic
        alloc
        array
        asciiz
        format
        preprocessor
    OPTIONS C_COMPILE_OPTIONS
)

bb_add_executable(steroids)

find_library(XMEMPOOL_LIBRARY NAMES xmempool)

target_compile_options(steroids PRIVATE ${C_COMPILE_OPTIONS})
target_sources(steroids PRIVATE
    main.c
    genid.c
    modules_manager.c
    "${CMAKE_SOURCE_DIR}/src/utils.c"
)

set_target_properties(steroids PROPERTIES
    C_STANDARD 11
    C_EXTENSIONS ON
    POSITION_INDEPENDENT_CODE 1
)

target_include_directories(steroids PRIVATE
    "${CMAKE_FIND_ROOT_PATH}/include"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/include"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(steroids PRIVATE
    ${ST_INTERNAL_MODULES_LIBS}
    ${XMEMPOOL_LIBRARY}
)