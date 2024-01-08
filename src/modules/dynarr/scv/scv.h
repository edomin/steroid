#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/dynarr.h"

st_dynarr_funcs_t st_dynarr_scv_funcs = {
    .dynarr_init               = st_dynarr_init,
    .dynarr_quit               = st_dynarr_quit,
    .dynarr_create             = st_dynarr_create,
    .dynarr_destroy            = st_dynarr_destroy,
    .dynarr_append             = st_dynarr_append,
    .dynarr_set                = st_dynarr_set,
    .dynarr_clear              = st_dynarr_clear,
    .dynarr_sort               = st_dynarr_sort,
    .dynarr_export             = st_dynarr_export,
    .dynarr_get                = st_dynarr_get,
    .dynarr_get_all            = st_dynarr_get_all,
    .dynarr_get_elements_count = st_dynarr_get_elements_count,
    .dynarr_is_empty           = st_dynarr_is_empty,
};

st_modfuncentry_t st_module_dynarr_scv_funcs[] = {
    {"init",               st_dynarr_init},
    {"quit",               st_dynarr_quit},
    {"create",             st_dynarr_create},
    {"destroy",            st_dynarr_destroy},
    {"append",             st_dynarr_append},
    {"set",                st_dynarr_set},
    {"clear",              st_dynarr_clear},
    {"sort",               st_dynarr_sort},
    {"export",             st_dynarr_export},
    {"get",                st_dynarr_get},
    {"get_all",            st_dynarr_get_all},
    {"get_elements_count", st_dynarr_get_elements_count},
    {"is_empty",           st_dynarr_is_empty},
    {NULL, NULL},
};
