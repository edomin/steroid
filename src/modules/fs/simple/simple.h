#ifndef ST_MODULES_FS_SIMPLE_SIMPLE_H
#define ST_MODULES_FS_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "steroids/fs.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/pathtools.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_fs_simple_logger_t;

typedef struct {
    st_modctx_t           *ctx;
    st_pathtools_resolve_t resolve;
} st_fs_simple_pathtools_t;

typedef struct {
    st_fs_simple_logger_t    logger;
    st_fs_simple_pathtools_t pathtools;
} st_fs_simple_t;

st_fs_funcs_t st_fs_simple_funcs = {
    .fs_init          = st_fs_init,
    .fs_quit          = st_fs_quit,
    .fs_get_file_type = st_fs_get_file_type,
    .fs_mkdir         = st_fs_mkdir,
};

#define FUNCS_COUNT 4
st_modfuncstbl_t st_module_fs_simple_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_fs_init"         , st_fs_init},
        {"st_fs_quit"         , st_fs_quit},
        {"st_fs_get_file_type", st_fs_get_file_type},
        {"st_fs_mkdir"        , st_fs_mkdir},
    }
};

#endif /* ST_MODULES_FS_SIMPLE_SIMPLE_H */
