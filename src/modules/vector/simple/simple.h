#ifndef ST_MODULES_VECTOR_SIMPLE_SIMPLE_H
#define ST_MODULES_VECTOR_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "steroids/vector.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_vector_simple_logger_t;

typedef struct {
    st_vector_simple_logger_t logger;
} st_vector_simple_t;

st_vector_funcs_t st_vector_simple_funcs = {
    .vector_init    = st_vector_init,
    .vector_quit    = st_vector_quit,
    .vector_rrotate = st_vector_rrotate,
    .vector_drotate = st_vector_drotate,
};

st_modfuncentry_t st_module_vector_simple_funcs[] = {
    {"init",    st_vector_init},
    {"quit",    st_vector_quit},
    {"rrotate", st_vector_rrotate},
    {"drotate", st_vector_drotate},
    {NULL, NULL},
};

#endif /* ST_MODULES_VECTOR_SIMPLE_SIMPLE_H */
