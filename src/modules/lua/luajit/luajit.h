#ifndef ST_MODULES_LOGGER_LUAJIT_H
#define ST_MODULES_LOGGER_LUAJIT_H

#include "config.h"
#include "types.h"
#include "steroids/lua.h"

st_lua_funcs_t st_lua_luajit_funcs = {
    .lua_init                   = st_lua_init,
    .lua_quit                   = st_lua_quit,
    .lua_get_logger_ctx         = st_lua_get_logger_ctx,
    .lua_get_opts_ctx           = st_lua_get_opts_ctx,
    .lua_run_string             = st_lua_run_string,
    .lua_run_file               = st_lua_run_file,
    .lua_get_state              = st_lua_get_state,
    .lua_create_userdata        = st_lua_create_userdata,
    .lua_create_metatable       = st_lua_create_metatable,
    .lua_set_metatable          = st_lua_set_metatable,
    .lua_push_bool              = st_lua_push_bool,
    .lua_set_nil_to_field       = st_lua_set_nil_to_field,
    .lua_set_integer_to_field   = st_lua_set_integer_to_field,
    .lua_set_cfunction_to_field = st_lua_set_cfunction_to_field,
    .lua_get_integer            = st_lua_get_integer,
    .lua_get_string             = st_lua_get_string,
    .lua_get_named_userdata     = st_lua_get_named_userdata,
    .lua_register_cfunction     = st_lua_register_cfunction,
    .lua_pop                    = st_lua_pop,
};

#define FUNCS_COUNT 19
st_modfuncstbl_t st_module_lua_luajit_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init",                   st_lua_init},
        {"quit",                   st_lua_quit},
        {"get_logger_ctx",         st_lua_get_logger_ctx},
        {"get_opts_ctx",           st_lua_get_opts_ctx},
        {"run",                    st_lua_run_file},
        {"run_string",             st_lua_run_string},
        {"run_file",               st_lua_run_file},
        {"get_state",              st_lua_get_state},
        {"create_userdata",        st_lua_create_userdata},
        {"create_metatable",       st_lua_create_metatable},
        {"set_metatable",          st_lua_set_metatable},
        {"push_bool",              st_lua_push_bool},
        {"set_nil_to_field",       st_lua_set_nil_to_field},
        {"set_integer_to_field",   st_lua_set_integer_to_field},
        {"set_cfunction_to_field", st_lua_set_cfunction_to_field},
        {"get_integer",            st_lua_get_integer},
        {"get_string",             st_lua_get_string},
        {"get_named_userdata",     st_lua_get_named_userdata},
        {"register_cfunction",     st_lua_register_cfunction},
        {"pop",                    st_lua_pop},
    }
};

#endif
