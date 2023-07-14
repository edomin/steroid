#ifndef ST_STEROIDS_PLUGIN_H
#define ST_STEROIDS_PLUGIN_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/plugin.h"

static st_modctx_t *st_plugin_init(st_modctx_t *fs_ctx, st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx, st_modctx_t *so_ctx, st_modctx_t *spcpaths_ctx,
 st_modctx_t *zip_ctx);
static void st_plugin_quit(st_modctx_t *plugin_ctx);

static bool st_plugin_load(st_modctx_t *plugin_ctx, const char *filename);
static bool st_plugin_memload(st_modctx_t *plugin_ctx, const void *data,
 size_t size);

#endif /* ST_STEROIDS_INI_H */
