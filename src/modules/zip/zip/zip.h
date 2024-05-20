#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/zip.h"
#include "steroids/types/modules/logger.h"

static st_zip_funcs_t st_zip_zip_funcs = {
    .zip_init              = st_zip_init,
    .zip_quit              = st_zip_quit,
    .zip_open              = st_zip_open,
    .zip_memopen           = st_zip_memopen,
    .zip_close             = st_zip_close,
    .zip_get_entries_count = st_zip_get_entries_count,
    .zip_get_entry_name    = st_zip_get_entry_name,
    .zip_get_entry_type    = st_zip_get_entry_type,
    .zip_extract_entry     = st_zip_extract_entry,
};

static st_modfuncentry_t st_module_zip_zip_funcs[] = {
    {"init"             , st_zip_init},
    {"quit"             , st_zip_quit},
    {"open"             , st_zip_open},
    {"memopen"          , st_zip_memopen},
    {"close"            , st_zip_close},
    {"get_entries_count", st_zip_get_entries_count},
    {"get_entry_name"   , st_zip_get_entry_name},
    {"get_entry_type"   , st_zip_get_entry_type},
    {"extract_entry"    , st_zip_extract_entry},
    {NULL, NULL},
};
