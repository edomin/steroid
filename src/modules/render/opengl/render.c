#include "render.h"

// #include <GL/gl.h>
// #include <GL/glu.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

// void (*glGenerateMipmap)(GLenum target);

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(render_opengl)
ST_MODULE_DEF_INIT_FUNC(render_opengl)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_render_opengl_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_render_import_functions(st_modctx_t *render_ctx,
 st_modctx_t *drawq_ctx, st_modctx_t *dynarr_ctx, st_modctx_t *gfxctx_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *sprite_ctx, st_modctx_t *texture_ctx,
 st_modctx_t *window_ctx) {
    st_render_opengl_t *module = render_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "render_opengl: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, create);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, empty);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, export_entry);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, add);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, sort);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, clear);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, create);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, destroy);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, append);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, clear);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, get_all);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, get_elements_count);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", gfxctx, make_current);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", gfxctx, swap_buffers);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", gfxctx, get_api);

    ST_LOAD_FUNCTION("render_opengl", glloader, NULL, get_proc_address);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", sprite, get_texture);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", sprite, get_width);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", sprite, get_height);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", sprite, export_uv);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", texture, bind);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", window, get_width);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", window, get_height);

    return true;
}

static st_modctx_t *st_render_init(st_modctx_t *drawq_ctx,
 st_modctx_t *dynarr_ctx, st_modctx_t *gfxctx_ctx, st_modctx_t *logger_ctx,
 st_modctx_t *sprite_ctx, st_modctx_t *texture_ctx, st_modctx_t *window_ctx) {
    st_modctx_t        *render_ctx;
    st_render_opengl_t *module;

    render_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_render_opengl_data, sizeof(st_render_opengl_t));

    if (!render_ctx)
        return NULL;

    render_ctx->funcs = &st_render_opengl_funcs;

    module = render_ctx->data;
    module->drawq.ctx = drawq_ctx;
    module->dynarr.ctx = dynarr_ctx;
    module->logger.ctx = logger_ctx;

    if (!st_texture_import_functions(render_ctx, drawq_ctx, dynarr_ctx,
     gfxctx_ctx, logger_ctx, sprite_ctx, texture_ctx, window_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, render_ctx);

        return NULL;
    }

    // glGenerateMipmap = module->glloader.get_proc_address(NULL,
    //  "glGenerateMipmap");

    module->logger.info(module->logger.ctx,
     "render_opengl: Render subsystem initialized.");

    return render_ctx;
}

static void st_render_quit(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;

    module->logger.info(module->logger.ctx,
     "render_opengl: Render subsystem destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, render_ctx);
}

static void st_render_put_sprite(st_modctx_t *render_ctx,
 const st_sprite_t *sprite, float x, float y, float z, float angle) {

}

static void st_render_process(st_modctx_t *render_ctx) {

}
