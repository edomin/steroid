#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"

typedef st_modctx_t *(*st_plugin_init_t)(st_modctx_t *fs_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *pathtools_ctx, st_modctx_t *so_ctx,
 st_modctx_t *spcpaths_ctx, st_modctx_t *zip_ctx);
typedef void (*st_plugin_quit_t)(st_modctx_t *plugin_ctx);

typedef bool (*st_plugin_load_t)(st_modctx_t *plugin_ctx, const char *filename,
 bool force);
typedef bool (*st_plugin_memload_t)(st_modctx_t *plugin_ctx, const void *data,
 size_t size, bool force);

typedef struct {
    st_plugin_init_t    plugin_init;
    st_plugin_quit_t    plugin_quit;
    st_plugin_load_t    plugin_load;
    st_plugin_memload_t plugin_memload;
} st_plugin_funcs_t;
