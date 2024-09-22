#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/fs.h"

static st_modfuncentry_t st_module_fs_simple_funcs[] = {
    {"init"         , st_fs_init},
    {"quit"         , st_fs_quit},
    {"get_file_type", st_fs_get_file_type},
    {"mkdir"        , st_fs_mkdir},
    {NULL, NULL},
};
