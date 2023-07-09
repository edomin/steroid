#ifndef ST_MODULES_PATHTOOLS_SIMPLE_SIMPLE_H
#define ST_MODULES_PATHTOOLS_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "steroids/pathtools.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
} st_pathtools_simple_logger_t;

typedef struct {
    st_pathtools_simple_logger_t logger;
} st_pathtools_simple_t;

st_pathtools_funcs_t st_pathtools_simple_funcs = {
    .pathtools_init           = st_pathtools_init,
    .pathtools_quit           = st_pathtools_quit,
    .pathtools_resolve        = st_pathtools_resolve,
    .pathtools_get_parent_dir = st_pathtools_get_parent_dir,
    .pathtools_concat         = st_pathtools_concat,
};

#define FUNCS_COUNT 5
st_modfuncstbl_t st_module_pathtools_simple_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_pathtools_init"          , st_pathtools_init},
        {"st_pathtools_quit"          , st_pathtools_quit},
        {"st_pathtools_resolve"       , st_pathtools_resolve},
        {"st_pathtools_get_parent_dir", st_pathtools_get_parent_dir},
        {"st_pathtools_concat"        , st_pathtools_concat},
    }
};

#endif /* ST_MODULES_PATHTOOLS_SIMPLE_SIMPLE_H */
