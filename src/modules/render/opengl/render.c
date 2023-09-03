#include "render.h"

#include <assert.h>

#include <GL/gl.h>
#include <GL/glu.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#include "glfuncs.inl" // NOLINT(llvm-include-order)
#include "batcher.inl"
#include "shader.inl"
#include "shdprog.inl"
#include "vao.inl"
#include "vertices.inl" // NOLINT(llvm-include-order)
#include "vbo.inl"
#include "vertattr.inl"

#define ERR_MSG_BUF_SIZE               1024
#define DEPTH_RANGE_NEAR_VAL           0.0
#define DEPTH_RANGE_FAR_VAL            1.0
#define VBO_COMPONENTS_PER_VERTEX      5

#define ATTR_POS_NAME                  "pos"
#define ATTR_POS_COMPONENTS_COUNT      3
#define ATTR_POS_OFFSET                0
#define ATTR_TEXCOORD_NAME             "vert_tex_coord"
#define ATTR_TEXCOORD_COMPONENTS_COUNT 2
#define ATTR_TEXCOORD_OFFSET           \
 (sizeof(float) * ATTR_POS_COMPONENTS_COUNT)

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
 st_modctx_t *drawq_ctx, st_modctx_t *dynarr_ctx, st_modctx_t *logger_ctx,
 st_modctx_t *sprite_ctx, st_modctx_t *texture_ctx, st_gfxctx_t *gfxctx) {
    st_render_opengl_t *module = render_ctx->data;
    st_gfxctx_get_ctx_t st_gfxctx_get_ctx;
    st_window_get_ctx_t st_window_get_ctx;
    st_modctx_t        *gfxctx_ctx;
    st_modctx_t        *window_ctx;
    st_window_t        *window;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "render_opengl: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    st_gfxctx_get_ctx = global_modsmgr_funcs.get_function(global_modsmgr,
     "gfxctx", NULL, "get_ctx");
    if (!st_gfxctx_get_ctx) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to load function \"get_ctx\" from module "
         "\"gfxctx\"\n");

        return false;
    }
    gfxctx_ctx = st_gfxctx_get_ctx(gfxctx);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", gfxctx, get_window);
    window = module->gfxctx.get_window(gfxctx);

    st_window_get_ctx = global_modsmgr_funcs.get_function(global_modsmgr,
     "window", NULL, "get_ctx");
    if (!st_window_get_ctx) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to load function \"get_ctx\" from module "
         "\"window\"\n");

        return false;
    }
    window_ctx = st_window_get_ctx(window);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, create);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, destroy);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, len);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, get_all);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, add);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, sort);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", drawq, clear);

    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, create);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, destroy);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, append);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, clear);
    ST_LOAD_FUNCTION_FROM_CTX("render_opengl", dynarr, get);
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
 st_modctx_t *dynarr_ctx, st_modctx_t *logger_ctx, st_modctx_t *sprite_ctx,
 st_modctx_t *texture_ctx, st_gfxctx_t *gfxctx) {
    st_modctx_t        *render_ctx;
    st_render_opengl_t *module;
    st_shader_t         shd_vert = 0;
    st_shader_t         shd_frag = 0;

    render_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_render_opengl_data, sizeof(st_render_opengl_t));

    if (!render_ctx)
        return NULL;

    render_ctx->funcs = &st_render_opengl_funcs;

    module = render_ctx->data;
    module->drawq.ctx = drawq_ctx;
    module->dynarr.ctx = dynarr_ctx;
    module->logger.ctx = logger_ctx;

    if (!st_render_import_functions(render_ctx, drawq_ctx, dynarr_ctx,
     logger_ctx, sprite_ctx, texture_ctx, gfxctx))
        goto import_fail;

    module->drawq.handle = module->drawq.create(module->drawq.ctx);
    if (!module->drawq.handle) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to create draw queue");

        goto drawq_fail;
    }

    if (!vertices_init(render_ctx)) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to init vertices array");

        goto vertices_fail;
    }

    if (!batcher_init(render_ctx)) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to init batcher");

        goto batcher_fail;
    }

    module->gfxctx.handle = gfxctx;
    module->window.handle = module->gfxctx.get_window(gfxctx);

    glfuncs_load_all(render_ctx);

    if (glapi_least(ST_GAPI_GL3))
        vao_init(render_ctx);

    if (glapi_least(ST_GAPI_GL2)) {
        vbo_init(render_ctx, VBO_COMPONENTS_PER_VERTEX);
        /* We have shader sources for only OpenGL 3.3 */
        assert(current_gapi == ST_GAPI_GL33);

        if (!shader_init(render_ctx, &shd_vert, SHD_VERTEX,
         VERTEX_SHADER_SOURCE_GL33)) {
            if (glapi_least(ST_GAPI_GL3))
                goto vert_fail;
        }

        if (shd_vert &&
         !shader_init(render_ctx, &shd_frag, SHD_FRAGMENT,
          FRAGMENT_SHADER_SOURCE_GL33)) {
            if (glapi_least(ST_GAPI_GL3))
                goto frag_fail;
        }

        if (shd_vert && shd_frag &&
         !shdprog_init(render_ctx, &shd_vert, &shd_frag)) {
            if (glapi_least(ST_GAPI_GL3))
                goto prog_fail;
        }

        if (module->shdprog && !vertattr_init(render_ctx, &module->posattr,
         &module->vbo, &module->shdprog, ATTR_POS_NAME,
         ATTR_POS_COMPONENTS_COUNT, ATTR_POS_OFFSET)) {
            if (glapi_least(ST_GAPI_GL3))
                goto posattr_fail;
        }

        if (module->shdprog && (module->posattr.handle != -1) &&
         !vertattr_init(render_ctx, &module->texcrdattr, &module->vbo,
         &module->shdprog, ATTR_TEXCOORD_NAME, ATTR_TEXCOORD_COMPONENTS_COUNT,
         ATTR_TEXCOORD_OFFSET)) {
            if (glapi_least(ST_GAPI_GL3))
                goto texcrdattr_fail;
        }

        shader_free(&shd_frag);
        shader_free(&shd_vert);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDepthRange(DEPTH_RANGE_NEAR_VAL, DEPTH_RANGE_FAR_VAL);
    glClearDepth(DEPTH_RANGE_FAR_VAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    module->logger.info(module->logger.ctx,
     "render_opengl: Render subsystem initialized");

    return render_ctx;

texcrdattr_fail:
    if (glapi_least(ST_GAPI_GL2))
        vertattr_free(&module->texcrdattr);
posattr_fail:
    if (glapi_least(ST_GAPI_GL2))
        shdprog_free(&module->shdprog);
prog_fail:
    if (glapi_least(ST_GAPI_GL2))
        shader_free(&shd_frag);
frag_fail:
    if (glapi_least(ST_GAPI_GL2))
        shader_free(&shd_vert);
vert_fail:
    if (glapi_least(ST_GAPI_GL2))
        vbo_free(&module->vbo);
    if (glapi_least(ST_GAPI_GL3))
        vao_free(&module->vao);
    batcher_free(&module->batcher);
batcher_fail:
    vertices_free(&module->vertices);
vertices_fail:
    module->drawq.destroy(module->drawq.handle);
drawq_fail:
import_fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, render_ctx);

    return NULL;
}

static void st_render_quit(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;

    if (glapi_least(ST_GAPI_GL2)) {
        vertattr_free(&module->texcrdattr);
        vertattr_free(&module->posattr);
        shdprog_free(&module->shdprog);
        vbo_free(&module->vbo);
    }
    if (glapi_least(ST_GAPI_GL3))
        vao_free(&module->vao);
    batcher_free(&module->batcher);
    vertices_free(&module->vertices);
    module->drawq.destroy(module->drawq.handle);

    module->logger.info(module->logger.ctx,
     "render_opengl: Render subsystem destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, render_ctx);
}

static void st_render_put_sprite(st_modctx_t *render_ctx,
 const st_sprite_t *sprite, float x, float y, float z, float scale,
 float angle) {
    st_render_opengl_t *module = render_ctx->data;

    module->drawq.add(module->drawq.handle, sprite, x, y, z, scale, angle);
}

static void st_render_process_queue(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;
    unsigned            window_width = module->window.get_width(
     module->window.handle);
    unsigned            window_height = module->window.get_height(
     module->window.handle);
    const st_drawrec_t *draw_entries = module->drawq.get_all(
     module->drawq.handle);
    size_t              draw_entries_count = module->drawq.len(
     module->drawq.handle);

    vertices_clear(&module->vertices);
    batcher_clear(&module->batcher);

    if (draw_entries_count == 0)
        return;

    module->drawq.sort(module->drawq.handle);
    for (size_t i = 0; i < draw_entries_count; i++) {
        const st_sprite_t  *sprite = draw_entries[i].sprite;
        const st_texture_t *texture = module->sprite.get_texture(sprite);
        unsigned            sprite_width = module->sprite.get_width(sprite);
        unsigned            sprite_height = module->sprite.get_height(sprite);
        float               pos_upper_left_x = draw_entries[i].x /
         (float)window_width * 2 - 1.0f;
        float               pos_upper_left_y = 1.0f - draw_entries[i].y /
         (float)window_height * 2;
        float               pos_upper_right_x = pos_upper_left_x +
         (float)sprite_width / (float)window_width * 2 * draw_entries[i].scale;
        float               pos_upper_right_y = pos_upper_left_y;
        float               pos_lower_left_x = pos_upper_left_x;
        float               pos_lower_left_y = pos_upper_left_y -
         (float)sprite_height / (float)window_height * 2 *
         draw_entries[i].scale;
        float               pos_lower_right_x = pos_upper_right_x;
        float               pos_lower_right_y = pos_lower_left_y;
        float               pos_z = draw_entries[i].z / (float)UINT16_MAX +
         0.5f; // NOLINT(readability-magic-numbers)
        st_uv_t             uv;

        batcher_process_texture(&module->batcher, texture);

        module->sprite.export_uv(sprite, &uv);

        vertices_add(&module->vertices, pos_upper_left_x, pos_upper_left_y,
         pos_z, uv.upper_left.u, uv.upper_left.v);
        vertices_add(&module->vertices, pos_upper_right_x, pos_upper_right_y,
         pos_z, uv.upper_right.u, uv.upper_right.v);
        vertices_add(&module->vertices, pos_lower_left_x, pos_lower_left_y,
         pos_z, uv.lower_left.u, uv.lower_left.v);
        vertices_add(&module->vertices, pos_upper_right_x, pos_upper_right_y,
         pos_z, uv.upper_right.u, uv.upper_right.v);
        vertices_add(&module->vertices, pos_lower_left_x, pos_lower_left_y,
         pos_z, uv.lower_left.u, uv.lower_left.v);
        vertices_add(&module->vertices, pos_lower_right_x, pos_lower_right_y,
         pos_z, uv.lower_right.u, uv.lower_right.v);
    }

    batcher_finalize(&module->batcher);
}

static void st_render_process(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;

    st_render_process_queue(render_ctx);

    module->gfxctx.make_current(module->gfxctx.handle);
    glClear((GLbitfield)GL_COLOR_BUFFER_BIT | (GLbitfield)GL_DEPTH_BUFFER_BIT);

    shdprog_use(&module->shdprog);
    vbo_bind(&module->vbo);
    vbo_set_vertices(&module->vbo, &module->vertices);
    vertattr_enable(&module->posattr);
    vertattr_enable(&module->texcrdattr);

    for (size_t i = 0; i < batcher_get_entries_count(&module->batcher); i++) {
        GLenum error;

        batcher_bind_texture(&module->batcher, i);

        glDrawArrays(GL_TRIANGLES,
         batcher_get_first_vertex_index(&module->batcher, i),
         batcher_get_vertices_count(&module->batcher, i));

        error = glGetError();
        if (error != GL_NO_ERROR) {
            module->logger.error(module->logger.ctx,
             "render_opengl: Unable to draw array: %s", gluErrorString(error));

            break;
        }
    }

    vertattr_disable(&module->texcrdattr);
    vertattr_disable(&module->posattr);
    vbo_unbind(&module->vbo);
    shdprog_unuse(&module->shdprog);
    module->gfxctx.swap_buffers(module->gfxctx.handle);
    module->drawq.clear(module->drawq.handle);
}
