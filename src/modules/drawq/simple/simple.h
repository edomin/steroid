#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/drawq.h"

static st_drawqctx_funcs_t st_drawq_simple_funcs = {
    .quit = st_drawq_quit,
    .create = st_drawq_create,
};

static st_modfuncentry_t st_module_drawq_simple_funcs[] = {
    {"init",         st_drawq_init},
    {"quit",         st_drawq_quit},
    {"create",       st_drawq_create},
    {"destroy",      st_drawq_destroy},
    {"len",          st_drawq_len},
    {"empty",        st_drawq_empty},
    {"export_entry", st_drawq_export_entry},
    {"get_all",      st_drawq_get_all},
    {"add",          st_drawq_add},
    {"sort",         st_drawq_sort},
    {"clear",        st_drawq_clear},
    {NULL, NULL},
};
