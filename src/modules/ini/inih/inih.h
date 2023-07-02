#ifndef ST_MODULES_INI_INIH_INIH_H
#define ST_MODULES_INI_INIH_INIH_H

#include "config.h" // IWYU pragma: keep
#include "types.h"
#include "steroids/ini.h"

st_ini_funcs_t st_ini_inih_funcs = {
    .ini_init           = st_ini_init,
    .ini_quit           = st_ini_quit,
    .ini_create         = st_ini_create,
    .ini_load           = st_ini_load,
    .ini_memload        = st_ini_memload,
    .ini_destroy        = st_ini_destroy,
    .ini_section_exists = st_ini_section_exists,
    .ini_key_exists     = st_ini_key_exists,
    .ini_get_str        = st_ini_get_str,
    .ini_delete_section = st_ini_delete_section,
    .ini_delete_key     = st_ini_delete_key,
    .ini_clear_section  = st_ini_clear_section,
    .ini_add_section    = st_ini_add_section,
    .ini_add_key        = st_ini_add_key,
    .ini_export         = st_ini_export,
    .ini_save           = st_ini_save,
};

#define FUNCS_COUNT 16
st_modfuncstbl_t st_module_ini_inih_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_ini_init"          , st_ini_init},
        {"st_ini_quit"          , st_ini_quit},
        {"st_ini_create"        , st_ini_create},
        {"st_ini_load"          , st_ini_load},
        {"st_ini_memload"       , st_ini_memload},
        {"st_ini_destroy"       , st_ini_destroy},
        {"st_ini_section_exists", st_ini_section_exists},
        {"st_ini_key_exists"    , st_ini_key_exists},
        {"st_ini_get_str"       , st_ini_get_str},
        {"st_ini_delete_section", st_ini_delete_section},
        {"st_ini_delete_key"    , st_ini_delete_key},
        {"st_ini_clear_section" , st_ini_clear_section},
        {"st_ini_add_section"   , st_ini_add_section},
        {"st_ini_add_key"       , st_ini_add_key},
        {"st_ini_export"        , st_ini_export},
        {"st_ini_save"          , st_ini_save},
    }
};

#endif /* ST_MODULES_INI_INIH_INIH_H */
