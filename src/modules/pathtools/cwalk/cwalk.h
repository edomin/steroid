#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/pathtools.h"

static st_modfuncentry_t st_module_pathtools_cwalk_funcs[] = {
    {"init"          , st_pathtools_init},
    {"quit"          , st_pathtools_quit},
    {"resolve"       , st_pathtools_resolve},
    {"get_parent_dir", st_pathtools_get_parent_dir},
    {"concat"        , st_pathtools_concat},
    {NULL, NULL},
};
