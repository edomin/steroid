#ifndef ST_STEROIDS_PLUGIN_H
#define ST_STEROIDS_PLUGIN_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/plugin.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_plugin_init(st_modctx_t *fs_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *pathtools_ctx, st_modctx_t *so_ctx,
 st_modctx_t *spcpaths_ctx, st_modctx_t *zip_ctx);
ST_VISIBILITY void st_plugin_quit(st_modctx_t *plugin_ctx);

ST_VISIBILITY bool st_plugin_load(st_modctx_t *plugin_ctx,
 const char *filename);
ST_VISIBILITY bool st_plugin_memload(st_modctx_t *plugin_ctx, const void *data,
 size_t size);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_INI_H */
