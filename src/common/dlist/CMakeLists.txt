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

bb_add_library(dlist STATIC)

target_compile_options(dlist PRIVATE ${C_COMPILE_OPTIONS})
target_sources(dlist PRIVATE dlist.c)

bb_set_c_std(dlist STD 11 EXTENSIONS)
