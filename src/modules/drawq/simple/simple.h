#ifndef ST_SRC_MODULES_DRAWQ_SIMPLE_SIMPLE_H
#define ST_SRC_MODULES_DRAWQ_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/drawq.h"

st_drawq_funcs_t st_drawq_simple_funcs = {
    .drawq_init         = st_drawq_init,
    .drawq_quit         = st_drawq_quit,
    .drawq_create       = st_drawq_create,
    .drawq_empty        = st_drawq_empty,
    .drawq_export_entry = st_drawq_export_entry,
    .drawq_add          = st_drawq_add,
    .drawq_sort         = st_drawq_sort,
    .drawq_clear        = st_drawq_clear,
};

st_modfuncentry_t st_module_drawq_simple_funcs[] = {
    {"init",         st_drawq_init},
    {"quit",         st_drawq_quit},
    {"create",       st_drawq_create},
    {"empty",        st_drawq_empty},
    {"export_entry", st_drawq_export_entry},
    {"add",          st_drawq_add},
    {"sort",         st_drawq_sort},
    {"clear",        st_drawq_clear},
    {NULL, NULL},
};

#endif /* ST_SRC_MODULES_DRAWQ_SIMPLE_SIMPLE_H */
