#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/matrix3x3.h"

st_matrix3x3_funcs_t st_matrix3x3_simple_funcs = {
    .matrix3x3_init        = st_matrix3x3_init,
    .matrix3x3_quit        = st_matrix3x3_quit,
    .matrix3x3_clone       = st_matrix3x3_clone,
    .matrix3x3_custom      = st_matrix3x3_custom,
    .matrix3x3_identity    = st_matrix3x3_identity,
    .matrix3x3_translation = st_matrix3x3_translation,
    .matrix3x3_scaling     = st_matrix3x3_scaling,
    .matrix3x3_rrotation   = st_matrix3x3_rrotation,
    .matrix3x3_drotation   = st_matrix3x3_drotation,
    .matrix3x3_rhshearing  = st_matrix3x3_rhshearing,
    .matrix3x3_dhshearing  = st_matrix3x3_dhshearing,
    .matrix3x3_rvshearing  = st_matrix3x3_rvshearing,
    .matrix3x3_dvshearing  = st_matrix3x3_dvshearing,
    .matrix3x3_apply       = st_matrix3x3_apply,
    .matrix3x3_translate   = st_matrix3x3_translate,
    .matrix3x3_scale       = st_matrix3x3_scale,
    .matrix3x3_rrotate     = st_matrix3x3_rrotate,
    .matrix3x3_drotate     = st_matrix3x3_drotate,
    .matrix3x3_rhshear     = st_matrix3x3_rhshear,
    .matrix3x3_dhshear     = st_matrix3x3_dhshear,
    .matrix3x3_rvshear     = st_matrix3x3_rvshear,
    .matrix3x3_dvshear     = st_matrix3x3_dvshear,
    .matrix3x3_get_data    = st_matrix3x3_get_data,
};

st_modfuncentry_t st_module_matrix3x3_simple_funcs[] = {
    {"init",        st_matrix3x3_init},
    {"quit",        st_matrix3x3_quit},
    {"clone",       st_matrix3x3_clone},
    {"custom",      st_matrix3x3_custom},
    {"identity",    st_matrix3x3_identity},
    {"translation", st_matrix3x3_translation},
    {"scaling",     st_matrix3x3_scaling},
    {"rrotation",   st_matrix3x3_rrotation},
    {"drotation",   st_matrix3x3_drotation},
    {"rhshearing",  st_matrix3x3_rhshearing},
    {"dhshearing",  st_matrix3x3_dhshearing},
    {"rvshearing",  st_matrix3x3_rvshearing},
    {"dvshearing",  st_matrix3x3_dvshearing},
    {"apply",       st_matrix3x3_apply},
    {"translate",   st_matrix3x3_translate},
    {"scale",       st_matrix3x3_scale},
    {"rrotate",     st_matrix3x3_rrotate},
    {"drotate",     st_matrix3x3_drotate},
    {"rhshear",     st_matrix3x3_rhshear},
    {"dhshear",     st_matrix3x3_dhshear},
    {"rvshear",     st_matrix3x3_rvshear},
    {"dvshear",     st_matrix3x3_dvshear},
    {"get_data",    st_matrix3x3_get_data},
    {NULL, NULL},
};
