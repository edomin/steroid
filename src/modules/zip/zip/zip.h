#ifndef ST_MODULES_ZIP_ZIP_ZIP_H
#define ST_MODULES_ZIP_ZIP_ZIP_H

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/zip.h"
#include "steroids/types/modules/logger.h"

st_zip_funcs_t st_zip_zip_funcs = {
    .zip_init              = st_zip_init,
    .zip_quit              = st_zip_quit,
    .zip_open              = st_zip_open,
    .zip_close             = st_zip_close,
    .zip_get_entries_count = st_zip_get_entries_count,
    .zip_get_entry_name    = st_zip_get_entry_name,
    .zip_extract_entry     = st_zip_extract_entry,
};

#define FUNCS_COUNT 7
st_modfuncstbl_t st_module_zip_zip_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_zip_init"             , st_zip_init},
        {"st_zip_quit"             , st_zip_quit},
        {"st_zip_open"             , st_zip_open},
        {"st_zip_close"            , st_zip_close},
        {"st_zip_get_entries_count", st_zip_get_entries_count},
        {"st_zip_get_entry_name"   , st_zip_get_entry_name},
        {"st_zip_extract_entry"    , st_zip_extract_entry},
    }
};

#endif /* ST_MODULES_ZIP_ZIP_ZIP_H */
