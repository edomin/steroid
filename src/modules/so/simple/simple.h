#ifndef ST_MODULES_SO_SIMPLE_SIMPLE_H
#define ST_MODULES_SO_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "steroids/so.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_so_simple_logger_t;

typedef struct {
    st_so_simple_logger_t logger;
} st_so_simple_t;

typedef struct st_so st_so;;

st_so_funcs_t st_so_simple_funcs = {
    .so_init = st_so_init,
    .so_quit = st_so_quit,
    .so_open = st_so_open,
    .so_memopen = st_so_memopen,
    .so_close = st_so_close,
    .so_load_symbol = st_so_load_symbol,
};

#define FUNCS_COUNT 6
st_modfuncstbl_t st_module_so_simple_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_so_init", st_so_init},
        {"st_so_quit", st_so_quit},
        {"st_so_open", st_so_open},
        {"st_so_memopen", st_so_memopen},
        {"st_so_close", st_so_close},
        {"st_so_load_symbol", st_so_load_symbol},
    }
};

#endif /* ST_MODULES_SO_SIMPLE_SIMPLE_H */
