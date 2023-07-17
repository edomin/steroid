#ifndef ST_STEROIDS_TYPES_MODULES_LUA_H
#define ST_STEROIDS_TYPES_MODULES_LUA_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"

typedef struct st_luastate_s st_luastate_t;

typedef st_modctx_t *(*st_lua_init_t)(st_modctx_t *logger_ctx,
 st_modctx_t *opts_ctx);
typedef void (*st_lua_quit_t)(st_modctx_t *lua_ctx);
typedef st_modctx_t *(*st_lua_get_logger_ctx_t)(st_modctx_t *lua_ctx);
typedef st_modctx_t *(*st_lua_get_opts_ctx_t)(st_modctx_t *lua_ctx);
typedef bool (*st_lua_run_string_t)(st_modctx_t *lua_ctx, const char *string);
typedef bool (*st_lua_run_file_t)(st_modctx_t *lua_ctx, const char *filename);

typedef st_luastate_t *(*st_lua_get_state_t)(st_modctx_t *lua_ctx);
typedef void *(*st_lua_create_userdata_t)(st_luastate_t *lua_state,
 size_t size);
typedef void (*st_lua_create_metatable_t)(st_luastate_t *lua_state,
 const char *name);
typedef void (*st_lua_set_metatable_t)(st_luastate_t *lua_state,
 const char *name);
typedef void (*st_lua_push_bool_t)(st_luastate_t *lua_state, bool val);
typedef void (*st_lua_set_nil_to_field_t)(st_luastate_t *lua_state,
 const char *name);
typedef void (*st_lua_set_integer_to_field_t)(st_luastate_t *lua_state,
 const char *name, ptrdiff_t integer);
typedef void (*st_lua_set_cfunction_to_field_t)(st_luastate_t *lua_state,
 const char *name, void *cfunction);
typedef void (*st_lua_set_copy_to_field_t)(st_luastate_t *lua_state,
 const char *name, int index);
typedef ptrdiff_t (*st_lua_get_integer_t)(st_luastate_t *lua_state, int index);
typedef const char *(*st_lua_get_string_t)(st_luastate_t *lua_state, int index);
typedef void *(*st_lua_get_named_userdata_t)(st_luastate_t *lua_state,
 int index, const char *name);
typedef void *(*st_lua_get_global_userdata_t)(st_luastate_t *lua_state,
 const char *name);
typedef void (*st_lua_register_cfunction_t)(st_luastate_t *lua_state,
 const char *name, void *cfunction);
typedef void (*st_lua_pop_t)(st_luastate_t *lua_state, size_t elements_count);

typedef struct {
    st_lua_init_t                   lua_init;
    st_lua_quit_t                   lua_quit;
    st_lua_get_logger_ctx_t         lua_get_logger_ctx;
    st_lua_get_opts_ctx_t           lua_get_opts_ctx;
    st_lua_run_string_t             lua_run_string;
    st_lua_run_file_t               lua_run_file;

    st_lua_get_state_t              lua_get_state;
    st_lua_create_userdata_t        lua_create_userdata;
    st_lua_create_metatable_t       lua_create_metatable;
    st_lua_set_metatable_t          lua_set_metatable;
    st_lua_push_bool_t              lua_push_bool;
    st_lua_set_nil_to_field_t       lua_set_nil_to_field;
    st_lua_set_integer_to_field_t   lua_set_integer_to_field;
    st_lua_set_cfunction_to_field_t lua_set_cfunction_to_field;
    st_lua_set_copy_to_field_t      lua_set_copy_to_field;
    st_lua_get_integer_t            lua_get_integer;
    st_lua_get_string_t             lua_get_string;
    st_lua_get_named_userdata_t     lua_get_named_userdata;
    st_lua_get_global_userdata_t    lua_get_global_userdata;
    st_lua_register_cfunction_t     lua_register_cfunction;
    st_lua_pop_t                    lua_pop;
} st_lua_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_LUA_H */
