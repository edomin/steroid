#ifndef ST_MODULES_SO_SIMPLE_SIMPLE_H
#define ST_MODULES_SO_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/so.h"
#include "steroids/types/list.h"
#include "steroids/types/modules/logger.h"

st_so_funcs_t st_so_simple_funcs = {
    .so_init        = st_so_init,
    .so_quit        = st_so_quit,
    .so_open        = st_so_open,
    .so_memopen     = st_so_memopen,
    .so_close       = st_so_close,
    .so_load_symbol = st_so_load_symbol,
};

st_modfuncentry_t st_module_so_simple_funcs[] = {
    {"init"       , st_so_init},
    {"quit"       , st_so_quit},
    {"open"       , st_so_open},
    {"memopen"    , st_so_memopen},
    {"close"      , st_so_close},
    {"load_symbol", st_so_load_symbol},
    {NULL, NULL},
};

#endif /* ST_MODULES_SO_SIMPLE_SIMPLE_H */
