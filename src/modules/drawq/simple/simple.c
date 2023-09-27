#include "simple.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE        1024
#define DYNARR_INITIAL_CAPACITY 16384

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(drawq_simple)
ST_MODULE_DEF_INIT_FUNC(drawq_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_drawq_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_drawq_import_functions(st_modctx_t *drawq_ctx,
 st_modctx_t *dynarr_ctx, st_modctx_t *logger_ctx, st_modctx_t *sprite_ctx) {
    st_drawq_simple_t *module = drawq_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "drawq_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, create);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, destroy);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, append);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, clear);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, sort);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, export);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, get_all);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, is_empty);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", dynarr, get_elements_count);

    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("drawq_simple", sprite, get_texture);

    return true;
}

static st_modctx_t *st_drawq_init(st_modctx_t *dynarr_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *sprite_ctx) {
    st_modctx_t       *drawq_ctx;
    st_drawq_simple_t *module;

    drawq_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_drawq_simple_data, sizeof(st_drawq_simple_t));

    if (!drawq_ctx)
        return NULL;

    module = drawq_ctx->data;

    drawq_ctx->funcs = &st_drawq_simple_funcs;

    module->dynarr.ctx = dynarr_ctx;
    module->logger.ctx = logger_ctx;
    module->sprite.ctx = sprite_ctx;

    if (!st_drawq_import_functions(drawq_ctx, dynarr_ctx, logger_ctx,
     sprite_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, drawq_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx,
     "drawq_simple: Draw queues mgr initialized");

    return drawq_ctx;
}

static void st_drawq_quit(st_modctx_t *drawq_ctx) {
    st_drawq_simple_t *module = drawq_ctx->data;

    module->logger.info(module->logger.ctx,
     "drawq_simple: Draw queues mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, drawq_ctx);
}

static st_drawq_t *st_drawq_create(const st_modctx_t *drawq_ctx) {
    st_drawq_simple_t *module = drawq_ctx->data;
    st_drawq_t        *drawq = malloc(sizeof(st_drawq_t));

    if (!drawq) {
        module->logger.error(module->logger.ctx,
         "drawq_simple: Unable to allocate memory for draw queue: %s",
         strerror(errno));

        return NULL;
    }

    drawq->entries = module->dynarr.create(module->dynarr.ctx,
     sizeof(st_drawrec_t), DYNARR_INITIAL_CAPACITY);
    if (!drawq->entries) {
        module->logger.error(module->logger.ctx,
         "drawq_simple: Unable to initialize dynamic array for draw queue");
        free(drawq);

        return NULL;
    }

    drawq->module = module;

    return drawq;
}

static void st_drawq_destroy(st_drawq_t *drawq) {
    drawq->module->dynarr.destroy(drawq->entries);
    free(drawq);
}

static size_t st_drawq_len(const st_drawq_t *drawq) {
    return drawq->module->dynarr.get_elements_count(drawq->entries);
}

static bool st_drawq_empty(const st_drawq_t *drawq) {
    return drawq->module->dynarr.is_empty(drawq->entries);
}

static bool st_drawq_export_entry(const st_drawq_t *drawq,
 st_drawrec_t *drawrec, size_t index) {
    return drawq->module->dynarr.export(drawq->entries, drawrec, index);
}

static const st_drawrec_t *st_drawq_get_all(const st_drawq_t *drawq) {
    return drawq->module->dynarr.get_all(drawq->entries);
}

static bool st_drawq_add(st_drawq_t *drawq, const st_sprite_t *sprite, float x,
 float y, float z, float hscale, float vscale, float angle) {
    st_drawrec_t entry = { sprite, x, y, z, hscale, vscale, angle };

    return drawq->module->dynarr.append(drawq->entries, &entry);
}

static int st_drawrec_cmp(const void *leftptr, const void *rightptr,
 void *userdata) {
    const st_drawrec_t *left = leftptr;
    const st_drawrec_t *right = rightptr;
    st_drawq_simple_t  *module = userdata;

    const st_texture_t *left_tex = module->sprite.get_texture(left->sprite);
    const st_texture_t *right_tex = module->sprite.get_texture(right->sprite);

    if (fabsf(left->z - right->z) <= FLT_EPSILON) {
        if (left_tex == right_tex)
            return 0;

        return (left_tex < right_tex) ? -1 : 1;
    }

    return (left->z > right->z) ? -1 : 1;
}

static bool st_drawq_sort(st_drawq_t *drawq) {
    return drawq->module->dynarr.sort(drawq->entries, st_drawrec_cmp,
     drawq->module);
}

static bool st_drawq_clear(st_drawq_t *drawq) {
    return drawq->module->dynarr.clear(drawq->entries);
}
