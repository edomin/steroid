#include "render.h"

#include <stdio.h>

#define METATABLE_NAME "render_ctx"

static st_modsmgr_t                    *global_modsmgr;
static st_modsmgr_funcs_t               global_modsmgr_funcs;

static st_render_init_t                 st_render_init;
static st_render_quit_t                 st_render_quit;
static st_render_put_sprite_t           st_render_put_sprite;
static st_render_put_sprite_rdangled_t  st_render_put_sprite_rdangled;
static st_render_put_sprite_dgangled_t  st_render_put_sprite_dgangled;
static st_render_put_sprite_rhsheared_t st_render_put_sprite_rhsheared;
static st_render_put_sprite_dhsheared_t st_render_put_sprite_dhsheared;
static st_render_put_sprite_rvsheared_t st_render_put_sprite_rvsheared;
static st_render_put_sprite_dvsheared_t st_render_put_sprite_dvsheared;
static st_render_process_t              st_render_process;

static st_lua_get_state_t               st_lua_get_state;
static st_lua_create_userdata_t         st_lua_create_userdata;
static st_lua_create_metatable_t        st_lua_create_metatable;
static st_lua_create_module_t           st_lua_create_module;
static st_lua_set_metatable_t           st_lua_set_metatable;
static st_lua_set_cfunction_to_field_t  st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t       st_lua_set_copy_to_field;
static st_lua_get_named_userdata_t      st_lua_get_named_userdata;
static st_lua_get_double_t              st_lua_get_double;
static st_lua_pop_t                     st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_render)
ST_MODULE_DEF_INIT_FUNC(luabind_render)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_render_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_render_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_render: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_render", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_render", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, put_sprite);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, put_sprite_rdangled);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, put_sprite_dgangled);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, put_sprite_rhsheared);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, put_sprite_dhsheared);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, put_sprite_rvsheared);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, put_sprite_dvsheared);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", render, process);

    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, get_double);
    ST_LOAD_GLOBAL_FUNCTION("luabind_render", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t         *luabind_ctx;
    st_luabind_render_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_render_data, sizeof(st_luabind_render_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_render_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_render: Render funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_render_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_render: Render funcs binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_render_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "angle_ctx");
    st_modctx_t *drawq_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "drawq_ctx");
    st_modctx_t *dynarr_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 3, "dynarr_ctx");
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 4, "logger_ctx");
    st_modctx_t *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 5, "matrix3x3_ctx"); // NOLINT(readability-magic-numbers)
    st_modctx_t *sprite_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 6, "sprite_ctx"); // NOLINT(readability-magic-numbers)
    st_modctx_t *texture_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 7, "texture_ctx"); // NOLINT(readability-magic-numbers)
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 8, "vec2_ctx"); // NOLINT(readability-magic-numbers)
    st_gfxctx_t *gfxctx = *(st_gfxctx_t **)st_lua_get_named_userdata(
     lua_state, 9, "gfxctx"); // NOLINT(readability-magic-numbers)

    *(st_modctx_t **)userdata = st_render_init(angle_ctx, drawq_ctx, dynarr_ctx,
     logger_ctx, matrix3x3_ctx, sprite_ctx, texture_ctx, vec2_ctx, gfxctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_render_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_render_quit(render_ctx);

    return 0;
}

static int st_render_put_sprite_bind(st_luastate_t *lua_state) {
    st_modctx_t       *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const st_sprite_t *sprite = *(st_sprite_t **)st_lua_get_named_userdata(
     lua_state, 2, "sprite");
    double             x = st_lua_get_double(lua_state, 3);
    double             y = st_lua_get_double(lua_state, 4);
    double             z = st_lua_get_double(lua_state, 5); // NOLINT(readability-magic-numbers)
    double             hscale = st_lua_get_double(lua_state, 6); // NOLINT(readability-magic-numbers)
    double             vscale = st_lua_get_double(lua_state, 7); // NOLINT(readability-magic-numbers)
    double             pivot_x = st_lua_get_double(lua_state, 8); // NOLINT(readability-magic-numbers)
    double             pivot_y = st_lua_get_double(lua_state, 9); // NOLINT(readability-magic-numbers)

    st_render_put_sprite(render_ctx, sprite, (float)x, (float)y, (float)z,
     (float)hscale, (float)vscale, (float)pivot_x, (float)pivot_y);

    return 0;
}

static int st_render_put_sprite_rdangled_bind(st_luastate_t *lua_state) {
    st_modctx_t       *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const st_sprite_t *sprite = *(st_sprite_t **)st_lua_get_named_userdata(
     lua_state, 2, "sprite");
    double             x = st_lua_get_double(lua_state, 3);
    double             y = st_lua_get_double(lua_state, 4);
    double             z = st_lua_get_double(lua_state, 5); // NOLINT(readability-magic-numbers)
    double             hscale = st_lua_get_double(lua_state, 6); // NOLINT(readability-magic-numbers)
    double             vscale = st_lua_get_double(lua_state, 7); // NOLINT(readability-magic-numbers)
    double             radians = st_lua_get_double(lua_state, 8); // NOLINT(readability-magic-numbers)
    double             pivot_x = st_lua_get_double(lua_state, 9); // NOLINT(readability-magic-numbers)
    double             pivot_y = st_lua_get_double(lua_state, 10); // NOLINT(readability-magic-numbers)

    st_render_put_sprite_rdangled(render_ctx, sprite, (float)x, (float)y,
     (float)z, (float)hscale, (float)vscale, (float)radians, (float)pivot_x,
     (float)pivot_y);

    return 0;
}

static int st_render_put_sprite_dgangled_bind(st_luastate_t *lua_state) {
    st_modctx_t       *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const st_sprite_t *sprite = *(st_sprite_t **)st_lua_get_named_userdata(
     lua_state, 2, "sprite");
    double             x = st_lua_get_double(lua_state, 3);
    double             y = st_lua_get_double(lua_state, 4);
    double             z = st_lua_get_double(lua_state, 5); // NOLINT(readability-magic-numbers)
    double             hscale = st_lua_get_double(lua_state, 6); // NOLINT(readability-magic-numbers)
    double             vscale = st_lua_get_double(lua_state, 7); // NOLINT(readability-magic-numbers)
    double             degrees = st_lua_get_double(lua_state, 8); // NOLINT(readability-magic-numbers)
    double             pivot_x = st_lua_get_double(lua_state, 9); // NOLINT(readability-magic-numbers)
    double             pivot_y = st_lua_get_double(lua_state, 10); // NOLINT(readability-magic-numbers)

    st_render_put_sprite_dgangled(render_ctx, sprite, (float)x, (float)y,
     (float)z, (float)hscale, (float)vscale, (float)degrees, (float)pivot_x,
     (float)pivot_y);

    return 0;
}

static int st_render_put_sprite_rhsheared_bind(st_luastate_t *lua_state) {
    st_modctx_t       *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const st_sprite_t *sprite = *(st_sprite_t **)st_lua_get_named_userdata(
     lua_state, 2, "sprite");
    double             x = st_lua_get_double(lua_state, 3);
    double             y = st_lua_get_double(lua_state, 4);
    double             z = st_lua_get_double(lua_state, 5); // NOLINT(readability-magic-numbers)
    double             hscale = st_lua_get_double(lua_state, 6); // NOLINT(readability-magic-numbers)
    double             vscale = st_lua_get_double(lua_state, 7); // NOLINT(readability-magic-numbers)
    double             radians = st_lua_get_double(lua_state, 8); // NOLINT(readability-magic-numbers)
    double             pivot_x = st_lua_get_double(lua_state, 9); // NOLINT(readability-magic-numbers)
    double             pivot_y = st_lua_get_double(lua_state, 10); // NOLINT(readability-magic-numbers)

    st_render_put_sprite_rhsheared(render_ctx, sprite, (float)x, (float)y,
     (float)z, (float)hscale, (float)vscale, (float)radians, (float)pivot_x,
     (float)pivot_y);

    return 0;
}

static int st_render_put_sprite_dhsheared_bind(st_luastate_t *lua_state) {
    st_modctx_t       *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const st_sprite_t *sprite = *(st_sprite_t **)st_lua_get_named_userdata(
     lua_state, 2, "sprite");
    double             x = st_lua_get_double(lua_state, 3);
    double             y = st_lua_get_double(lua_state, 4);
    double             z = st_lua_get_double(lua_state, 5); // NOLINT(readability-magic-numbers)
    double             hscale = st_lua_get_double(lua_state, 6); // NOLINT(readability-magic-numbers)
    double             vscale = st_lua_get_double(lua_state, 7); // NOLINT(readability-magic-numbers)
    double             degrees = st_lua_get_double(lua_state, 8); // NOLINT(readability-magic-numbers)
    double             pivot_x = st_lua_get_double(lua_state, 9); // NOLINT(readability-magic-numbers)
    double             pivot_y = st_lua_get_double(lua_state, 10); // NOLINT(readability-magic-numbers)

    st_render_put_sprite_dhsheared(render_ctx, sprite, (float)x, (float)y,
     (float)z, (float)hscale, (float)vscale, (float)degrees, (float)pivot_x,
     (float)pivot_y);

    return 0;
}

static int st_render_put_sprite_rvsheared_bind(st_luastate_t *lua_state) {
    st_modctx_t       *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const st_sprite_t *sprite = *(st_sprite_t **)st_lua_get_named_userdata(
     lua_state, 2, "sprite");
    double             x = st_lua_get_double(lua_state, 3);
    double             y = st_lua_get_double(lua_state, 4);
    double             z = st_lua_get_double(lua_state, 5); // NOLINT(readability-magic-numbers)
    double             hscale = st_lua_get_double(lua_state, 6); // NOLINT(readability-magic-numbers)
    double             vscale = st_lua_get_double(lua_state, 7); // NOLINT(readability-magic-numbers)
    double             radians = st_lua_get_double(lua_state, 8); // NOLINT(readability-magic-numbers)
    double             pivot_x = st_lua_get_double(lua_state, 9); // NOLINT(readability-magic-numbers)
    double             pivot_y = st_lua_get_double(lua_state, 10); // NOLINT(readability-magic-numbers)

    st_render_put_sprite_rvsheared(render_ctx, sprite, (float)x, (float)y,
     (float)z, (float)hscale, (float)vscale, (float)radians, (float)pivot_x,
     (float)pivot_y);

    return 0;
}

static int st_render_put_sprite_dvsheared_bind(st_luastate_t *lua_state) {
    st_modctx_t       *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const st_sprite_t *sprite = *(st_sprite_t **)st_lua_get_named_userdata(
     lua_state, 2, "sprite");
    double             x = st_lua_get_double(lua_state, 3);
    double             y = st_lua_get_double(lua_state, 4);
    double             z = st_lua_get_double(lua_state, 5); // NOLINT(readability-magic-numbers)
    double             hscale = st_lua_get_double(lua_state, 6); // NOLINT(readability-magic-numbers)
    double             vscale = st_lua_get_double(lua_state, 7); // NOLINT(readability-magic-numbers)
    double             degrees = st_lua_get_double(lua_state, 8); // NOLINT(readability-magic-numbers)
    double             pivot_x = st_lua_get_double(lua_state, 9); // NOLINT(readability-magic-numbers)
    double             pivot_y = st_lua_get_double(lua_state, 10); // NOLINT(readability-magic-numbers)

    st_render_put_sprite_dvsheared(render_ctx, sprite, (float)x, (float)y,
     (float)z, (float)hscale, (float)vscale, (float)degrees, (float)pivot_x,
     (float)pivot_y);

    return 0;
}

static int st_render_process_bind(st_luastate_t *lua_state) {
    st_modctx_t *render_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_render_process(render_ctx);

    return 0;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_render_t *module = luabind_ctx->data;
    st_luastate_t       *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Render");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_render_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_render_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_render_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "put_sprite",
     st_render_put_sprite_bind);
    st_lua_set_cfunction_to_field(lua_state, "put_sprite_rdangled",
     st_render_put_sprite_rdangled_bind);
    st_lua_set_cfunction_to_field(lua_state, "put_sprite_dgangled",
     st_render_put_sprite_dgangled_bind);
    st_lua_set_cfunction_to_field(lua_state, "put_sprite_rhsheared",
     st_render_put_sprite_rhsheared_bind);
    st_lua_set_cfunction_to_field(lua_state, "put_sprite_dhsheared",
     st_render_put_sprite_dhsheared_bind);
    st_lua_set_cfunction_to_field(lua_state, "put_sprite_rvsheared",
     st_render_put_sprite_rvsheared_bind);
    st_lua_set_cfunction_to_field(lua_state, "put_sprite_dvsheared",
     st_render_put_sprite_dvsheared_bind);
    st_lua_set_cfunction_to_field(lua_state, "process", st_render_process_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);

    st_lua_pop(lua_state, 1);
}
