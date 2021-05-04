set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(ST_INSTALL_DIR "${CMAKE_SOURCE_DIR}/linux64")
set(ST_TARGET_TRIPLET x86_64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH /usr/local/${ST_TARGET_TRIPLET})

set(CMAKE_C_COMPILER x86_64-linux-gnu-gcc)
set(PKG_CONFIG_EXECUTABLE x86_64-linux-gnu-pkg-config)

function(st_target_set_fpu_options ST_TARGET)
    target_compile_options(${ST_TARGET} PRIVATE
        -mmmx -msse -msse2 -mfpmath=sse
    )
endfunction()

#set(ENV{PKG_CONFIG_LIBDIR} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig/:/usr/lib/x86_64-linux-gnu/pkgconfig)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
