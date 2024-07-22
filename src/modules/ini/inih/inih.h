#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"
#include "steroids/ini.h"

static st_inictx_funcs_t st_ini_inih_funcs = {
    .ini_init    = st_ini_init,
    .ini_quit    = st_ini_quit,
    .ini_create  = st_ini_create,
    .ini_load    = st_ini_load,
    .ini_memload = st_ini_memload,
};

static st_modfuncentry_t st_module_ini_inih_funcs[] = {
    {"init"          , st_ini_init},
    {"quit"          , st_ini_quit},
    {"create"        , st_ini_create},
    {"load"          , st_ini_load},
    {"memload"       , st_ini_memload},
    {"destroy"       , st_ini_destroy},
    {"section_exists", st_ini_section_exists},
    {"key_exists"    , st_ini_key_exists},
    {"get_str"       , st_ini_get_str},
    {"fill_str"      , st_ini_fill_str},
    {"delete_section", st_ini_delete_section},
    {"delete_key"    , st_ini_delete_key},
    {"clear_section" , st_ini_clear_section},
    {"add_section"   , st_ini_add_section},
    {"add_key"       , st_ini_add_key},
    {"export"        , st_ini_export},
    {"save"          , st_ini_save},
    {NULL, NULL},
};
