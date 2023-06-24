#ifndef ST_MODULES_SO_SIMPLE_SIMPLE_H
#define ST_MODULES_SO_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "steroids/so.h"
#include "steroids/types/list.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_so_simple_logger_t;

typedef struct {
    st_so_simple_logger_t logger;
    st_slist_t            opened_handles;
} st_so_simple_t;

st_so_funcs_t st_so_simple_funcs = {
    .so_init           = st_so_init,
    .so_quit           = st_so_quit,
    .so_load_module    = st_so_load_module,
    .so_memload_module = st_so_memload_module,
};

#define FUNCS_COUNT 4
st_modfuncstbl_t st_module_so_simple_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_so_init"          , st_so_init},
        {"st_so_quit"          , st_so_quit},
        {"st_so_load_module"   , st_so_load_module},
        {"st_so_memload_module", st_so_memload_module},
    }
};

#endif /* ST_MODULES_SO_SIMPLE_SIMPLE_H */
