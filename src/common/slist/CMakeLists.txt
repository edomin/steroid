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

bb_add_library(slist STATIC)

target_compile_options(slist PRIVATE ${C_COMPILE_OPTIONS})
target_sources(slist PRIVATE slist.c)

bb_set_c_std(slist STD 11 EXTENSIONS)
