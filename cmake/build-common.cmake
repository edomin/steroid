function(st_process_flag_availability ST_FLAG ST_HAVE_FLAG ST_FLAG_VALUE)
    check_c_compiler_flag(${ST_FLAG} ${ST_HAVE_FLAG})
    if (${ST_HAVE_FLAG})
        set(${ST_FLAG_VALUE} ${ST_FLAG})
    else()
        set(${ST_FLAG_VALUE} "")
    endif()
endfunction()

if(ST_MORE_WARNINGS)
    include(CheckCCompilerFlag)
    st_process_flag_availability(
        "-Walloc-zero" ST_HAVE_WALLOC_ZERO ST_WALLOC_ZERO
    )
    st_process_flag_availability("-Walloca" ST_HAVE_WALLOCA ST_WALLOCA)
    st_process_flag_availability(
        "-Wdangling-else" ST_HAVE_WDANGLING_ELSE ST_WDANGLING_ELSE
    )
    st_process_flag_availability(
        "-Wduplicated-branches"
        ST_HAVE_WDUPLICATED_BRANCHES
        ST_WDUPLICATED_BRANCHES
    )
    st_process_flag_availability(
        "-Wformat-overflow=2" ST_HAVE_WFORMAT_OVERFLOW_2 ST_WFORMAT_OVERFLOW_2
    )
    st_process_flag_availability(
        "-Wif-not-aligned" ST_HAVE_WIF_NOT_ALIGNED ST_WIF_NOT_ALIGNED
    )
    st_process_flag_availability(
        "-Wstringop-truncation"
        ST_HAVE_WSTRINGOP_TRUNCATION
        ST_WSTRINGOP_TRUNCATION
    )
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
        -Wall -Wextra -Wno-unused-parameter -Wshadow -Werror
    )
    if (ST_MORE_WARNINGS)
        target_compile_options(${ST_TARGET} PRIVATE
            -Waggregate-return ${ST_WALLOC_ZERO} ${ST_WALLOCA}
            -Wbad-function-cast -Wcast-align -Wcast-qual -Wconversion
            ${ST_WDANGLING_ELSE} -Wdeclaration-after-statement
            -Wdouble-promotion ${ST_WDUPLICATED_BRANCHES} -Wduplicated-cond
            -Wfloat-equal ${ST_WFORMAT_OVERFLOW_2} -Wformat-nonliteral
            -Wformat-security -Wformat-signedness ${ST_WIF_NOT_ALIGNED}
            -Winit-self -Winline -Wjump-misses-init -Wlogical-op
            -Wnested-externs -Wnonnull -Wnonnull-compare -Wnull-dereference
            -Wold-style-definition -Wpointer-arith -Wredundant-decls
            -Wshift-overflow=2 -Wstrict-prototypes ${ST_WSTRINGOP_TRUNCATION}
            -Wswitch-default -Wswitch-enum -Wundef -Wuninitialized
            -Wunsafe-loop-optimizations -Wunsuffixed-float-constants
            -Wvector-operation-performance -Wvla -Wwrite-strings
        )
    endif()
    if (COMMAND st_target_set_fpu_options)
        st_target_set_fpu_options(${ST_TARGET})
    endif()
endfunction()
