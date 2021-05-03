if(ST_MORE_WARNINGS)
    st_process_flag_availability(ST_WALLOC_ZERO "-Walloc-zero" "")
    st_process_flag_availability(ST_WALLOCA "-Walloca" "")
    st_process_flag_availability(ST_WARITH_CONVERSION "-Warith-conversion" "")
    st_process_flag_availability(ST_WDANGLING_ELSE "-Wdangling-else" "")
    st_process_flag_availability(ST_WDUPLICATED_BRANCHES
        "-Wduplicated-branches" "")
    st_process_flag_availability(ST_WFORMAT_OVERFLOW_2 "-Wformat-overflow=2"
        "")
    st_process_flag_availability(ST_WIF_NOT_ALIGNED "-Wif-not-aligned" "")
    st_process_flag_availability(ST_WSTRINGOP_TRUNCATION
        "-Wstringop-truncation" "")
    st_process_flag_availability(ST_WZERO_LENGTH_BOUNDS
        "-Wzero-length-bounds" "")
endif()

function(st_target_set_common ST_TARGET)
    if(ST_HAVE_LTO)
        set_target_properties(${ST_TARGET} PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE
        )
    endif()
    if(ST_HAVE_FLTO)
        target_compile_options(${ST_TARGET} PRIVATE -flto)
    endif()
    set_target_properties(${ST_TARGET} PROPERTIES
        C_STANDARD 11
        C_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE 1
    )
    target_include_directories(${ST_TARGET} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    )
    target_compile_options(${ST_TARGET} PRIVATE
        -Wall -Wextra -Wshadow -D_DEFAULT_SOURCE
    )
    if (ST_WERROR)
        target_compile_options(${ST_TARGET} PRIVATE -Werror)
    endif()
    if (ST_MORE_WARNINGS)
        target_compile_options(${ST_TARGET} PRIVATE
            -Waggregate-return ${ST_WALLOC_ZERO} ${ST_WALLOCA}
            ${ST_WARITH_CONVERSION} -Wbad-function-cast -Wcast-align
            -Wcast-qual -Wconversion ${ST_WDANGLING_ELSE}
            -Wdeclaration-after-statement -Wdouble-promotion
            ${ST_WDUPLICATED_BRANCHES} -Wduplicated-cond -Wfloat-equal
            ${ST_WFORMAT_OVERFLOW_2} -Wformat-nonliteral -Wformat-security
            -Wformat-signedness -Wformat-y2k ${ST_WIF_NOT_ALIGNED}
            -Wimplicit-fallthrough=5 -Winit-self -Winline -Winvalid-pch
            -Wjump-misses-init -Wlogical-op -Wmissing-include-dirs
            -Wnested-externs -Wnonnull -Wnonnull-compare -Wnull-dereference
            -Wold-style-definition -Wpacked -Wpointer-arith -Wredundant-decls
            -Wshift-overflow=2 -Wstrict-overflow=4 -Wstrict-prototypes
            -Wstringop-overflow=2 ${ST_WSTRINGOP_TRUNCATION}
            -Wsuggest-attribute=cold -Wsuggest-attribute=const
            -Wsuggest-attribute=format -Wsuggest-attribute=malloc
            -Wsuggest-attribute=noreturn -Wsuggest-attribute=pure
            -Wswitch-default -Wswitch-enum -Wtrampolines -Wundef
            -Wuninitialized -Wunknown-pragmas -Wunsafe-loop-optimizations
            -Wunsuffixed-float-constants -Wvector-operation-performance -Wvla
            -Wwrite-strings ${ST_WZERO_LENGTH_BOUNDS}
        )
    endif()
    if (COMMAND st_target_set_fpu_options)
        st_target_set_fpu_options(${ST_TARGET})
    endif()
endfunction()
