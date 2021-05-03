function(st_capitalize ST_STR ST_STR_CAPITALIZED)
    string(SUBSTRING ${ST_STR} 0 1 ST_FIRST_LETTER)
    string(TOUPPER ${ST_FIRST_LETTER} ST_FIRST_LETTER_CAPITALIZED)
    string(SUBSTRING ${ST_STR} 1 -1 ST_REMAINING_LETTERS)

    set(${ST_STR_CAPITALIZED}
        "${ST_FIRST_LETTER_CAPITALIZED}${ST_REMAINING_LETTERS}"
        PARENT_SCOPE
    )
endfunction()

# We dont use check_c_compiler_flag because We want more pretty configure
# output with printed checked option
function(st_process_flag_availability ST_FLAG_VALUE ST_FLAG ST_FALLBACK_FLAG)
    if (DEFINED ${ST_FLAG_VALUE})
        return()
    endif()
    message(STATUS "Check if compiler accepts ${ST_FLAG}")
    file(WRITE "${CMAKE_BINARY_DIR}/trycompile.c"
        "int main(int argc, char **argv){return 0;}")
    try_compile(ST_HAVE_FLAG ${CMAKE_BINARY_DIR}
        "${CMAKE_BINARY_DIR}/trycompile.c"
        COMPILE_DEFINITIONS ${ST_FLAG}
        C_STANDARD ${ST_C_STD}
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS ${ST_C_EXTENCIONS}
    )
    file(REMOVE "${CMAKE_BINARY_DIR}/trycompile.c")
    if (${ST_HAVE_FLAG})
        message(STATUS "Check if compiler accepts ${ST_FLAG} - yes")
        set(${ST_FLAG_VALUE} ${ST_FLAG} CACHE INTERNAL "")
    else()
        message(STATUS "Check if compiler accepts ${ST_FLAG} - no")
        set(${ST_FLAG_VALUE} ${ST_FALLBACK_FLAG} CACHE INTERNAL "")
    endif()
endfunction()
