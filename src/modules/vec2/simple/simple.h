#pragma once

#include "config.h" // IWYU pragma: keep
#include "steroids/vec2.h"
#include "steroids/types/modules/angle.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_vec2_simple_logger_t;

typedef struct {
    st_modctx_t             *ctx;
    st_angle_rtod_t          rtod;
    st_angle_dtor_t          dtor;
    st_angle_rnormalize360_t rnormalize360;
    st_angle_rdsin_t         rdsin;
    st_angle_rdcos_t         rdcos;
    st_angle_rdacos_t        rdacos;
} st_vec2_simple_angle_t;

typedef struct {
    st_vec2_simple_logger_t logger;
    st_vec2_simple_angle_t  angle;
} st_vec2_simple_t;

st_vec2_funcs_t st_vec2_simple_funcs = {
    .vec2_init               = st_vec2_init,
    .vec2_quit               = st_vec2_quit,
    .vec2_add                = st_vec2_add,
    .vec2_sum                = st_vec2_sum,
    .vec2_sub                = st_vec2_sub,
    .vec2_diff               = st_vec2_diff,
    .vec2_mul                = st_vec2_mul,
    .vec2_product            = st_vec2_product,
    .vec2_len                = st_vec2_len,
    .vec2_distance           = st_vec2_distance,
    .vec2_normalize          = st_vec2_normalize,
    .vec2_unit               = st_vec2_unit,
    .vec2_dot_product        = st_vec2_dot_product,
    .vec2_rangle             = st_vec2_rangle,
    .vec2_dangle             = st_vec2_dangle,
    .vec2_rrotate            = st_vec2_rrotate,
    .vec2_rrotation          = st_vec2_rrotation,
    .vec2_drotate            = st_vec2_drotate,
    .vec2_drotation          = st_vec2_drotation,
    .vec2_rotate90           = st_vec2_rotate90,
    .vec2_rotation90         = st_vec2_rotation90,
    .vec2_rotate180          = st_vec2_rotate180,
    .vec2_rotation180        = st_vec2_rotation180,
    .vec2_rotate270          = st_vec2_rotate270,
    .vec2_rotation270        = st_vec2_rotation270,
    .vec2_default_basis_xvec = st_vec2_default_basis_xvec,
    .vec2_default_basis_yvec = st_vec2_default_basis_yvec,
};

st_modfuncentry_t st_module_vec2_simple_funcs[] = {
    {"init",               st_vec2_init},
    {"quit",               st_vec2_quit},
    {"add",                st_vec2_add},
    {"sum",                st_vec2_sum},
    {"sub",                st_vec2_sub},
    {"diff",               st_vec2_diff},
    {"mul",                st_vec2_mul},
    {"product",            st_vec2_product},
    {"len",                st_vec2_len},
    {"distance",           st_vec2_distance},
    {"normalize",          st_vec2_normalize},
    {"unit",               st_vec2_unit},
    {"dot_product",        st_vec2_dot_product},
    {"rangle",             st_vec2_rangle},
    {"dangle",             st_vec2_dangle},
    {"rrotate",            st_vec2_rrotate},
    {"rrotation",          st_vec2_rrotation},
    {"drotate",            st_vec2_drotate},
    {"drotation",          st_vec2_drotation},
    {"rotate90",           st_vec2_rotate90},
    {"rotation90",         st_vec2_rotation90},
    {"rotate180",          st_vec2_rotate180},
    {"rotation180",        st_vec2_rotation180},
    {"rotate270",          st_vec2_rotate270},
    {"rotation270",        st_vec2_rotation270},
    {"default_basis_xvec", st_vec2_default_basis_xvec},
    {"default_basis_yvec", st_vec2_default_basis_yvec},
    {NULL, NULL},
};
