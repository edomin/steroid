#ifndef ST_STEROIDS_LUA_H
#define ST_STEROIDS_LUA_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/lua.h"

static st_modctx_t *st_lua_init(st_modctx_t *logger_ctx,
 st_modctx_t *opts_ctx);
static void st_lua_quit(st_modctx_t *lua_ctx);
static st_modctx_t *st_lua_get_logger_ctx(st_modctx_t *lua_ctx);
static st_modctx_t *st_lua_get_opts_ctx(st_modctx_t *lua_ctx);
static bool st_lua_run_string(st_modctx_t *lua_ctx, const char *string);
static bool st_lua_run_file(st_modctx_t *lua_ctx, const char *filename);

static st_luastate_t *st_lua_get_state(st_modctx_t *lua_ctx);
static void *st_lua_create_userdata(st_luastate_t *lua_state, size_t size);
static void st_lua_create_metatable(st_luastate_t *lua_state, const char *name);
static void st_lua_set_metatable(st_luastate_t *lua_state, const char *name);
static void st_lua_push_bool(st_luastate_t *lua_state, bool val);
static void st_lua_set_nil_to_field(st_luastate_t *lua_state, const char *name);
static void st_lua_set_integer_to_field(st_luastate_t *lua_state,
 const char *name, ptrdiff_t integer);
static void st_lua_set_cfunction_to_field(st_luastate_t *lua_state,
 const char *name, void *cfunction);
static void st_lua_set_copy_to_field(st_luastate_t *lua_state, const char *name,
 int index);
static ptrdiff_t st_lua_get_integer(st_luastate_t *lua_state, int index);
static const char *st_lua_get_string(st_luastate_t *lua_state, int index);
static void *st_lua_get_named_userdata(st_luastate_t *lua_state, int index,
 const char *name);
static void *st_lua_get_global_userdata(st_luastate_t *lua_state,
 const char *name);
static void st_lua_register_cfunction(st_luastate_t *lua_state,
 const char *name, void *cfunction);
static void st_lua_pop(st_luastate_t *lua_state, size_t elements_count);

#endif
