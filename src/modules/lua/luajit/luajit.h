#ifndef ST_MODULES_LOGGER_LUAJIT_H
#define ST_MODULES_LOGGER_LUAJIT_H

#include "config.h"
#include "types.h"
#include "steroids/lua.h"

st_lua_funcs_t st_lua_luajit_funcs = {
    .lua_init                       = st_lua_init,
    .lua_quit                       = st_lua_quit,
    .lua_run_string                 = st_lua_run_string,
    .lua_run_file                   = st_lua_run_file,
    .lua_get_state                  = st_lua_get_state,
    .lua_create_userdata            = st_lua_create_userdata,
    .lua_create_metatable           = st_lua_create_metatable,
    .lua_create_module              = st_lua_create_module,
    .lua_set_metatable              = st_lua_set_metatable,
    .lua_push_bool                  = st_lua_push_bool,
    .lua_push_integer               = st_lua_push_integer,
    .lua_push_nil                   = st_lua_push_nil,
    .lua_push_string                = st_lua_push_string,
    .lua_set_nil_to_field           = st_lua_set_nil_to_field,
    .lua_set_integer_to_field       = st_lua_set_integer_to_field,
    .lua_set_cfunction_to_field     = st_lua_set_cfunction_to_field,
    .lua_set_copy_to_field          = st_lua_set_copy_to_field,
    .lua_get_bool                   = st_lua_get_bool,
    .lua_get_char                   = st_lua_get_char,
    .lua_get_integer                = st_lua_get_integer,
    .lua_get_lstring_or_null        = st_lua_get_lstring_or_null,
    .lua_get_string                 = st_lua_get_string,
    .lua_get_string_or_null         = st_lua_get_string_or_null,
    .lua_get_named_userdata         = st_lua_get_named_userdata,
    .lua_get_named_userdata_or_null = st_lua_get_named_userdata_or_null,
    .lua_get_global_userdata        = st_lua_get_global_userdata,
    .lua_register_cfunction         = st_lua_register_cfunction,
    .lua_pop                        = st_lua_pop,
    .lua_raise_error                = st_lua_raise_error,
};

st_modfuncentry_t st_module_lua_luajit_funcs[] = {
    {"init",                       st_lua_init},
    {"quit",                       st_lua_quit},
    {"run",                        st_lua_run_file},
    {"run_string",                 st_lua_run_string},
    {"run_file",                   st_lua_run_file},
    {"get_state",                  st_lua_get_state},
    {"create_userdata",            st_lua_create_userdata},
    {"create_metatable",           st_lua_create_metatable},
    {"create_module",              st_lua_create_module},
    {"set_metatable",              st_lua_set_metatable},
    {"push_bool",                  st_lua_push_bool},
    {"push_integer",               st_lua_push_integer},
    {"push_nil",                   st_lua_push_nil},
    {"push_string",                st_lua_push_string},
    {"set_nil_to_field",           st_lua_set_nil_to_field},
    {"set_integer_to_field",       st_lua_set_integer_to_field},
    {"set_cfunction_to_field",     st_lua_set_cfunction_to_field},
    {"set_copy_to_field",          st_lua_set_copy_to_field},
    {"get_bool",                   st_lua_get_bool},
    {"get_char",                   st_lua_get_char},
    {"get_integer",                st_lua_get_integer},
    {"get_lstring_or_null",        st_lua_get_lstring_or_null},
    {"get_string",                 st_lua_get_string},
    {"get_string_or_null",         st_lua_get_string_or_null},
    {"get_named_userdata",         st_lua_get_named_userdata},
    {"get_named_userdata_or_null", st_lua_get_named_userdata_or_null},
    {"get_global_userdata",        st_lua_get_global_userdata},
    {"register_cfunction",         st_lua_register_cfunction},
    {"pop",                        st_lua_pop},
    {"raise_error",                st_lua_raise_error},
    {NULL, NULL},
};

#endif
