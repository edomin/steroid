#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/plugin.h"

static st_pluginctx_t *st_plugin_init(st_fsctx_t *fs_ctx,
 struct st_loggerctx_s *logger_ctx, st_pathtoolsctx_t *pathtools_ctx,
 st_soctx_t *so_ctx, st_spcpathsctx_t *spcpaths_ctx, st_zipctx_t *zip_ctx);
static void st_plugin_quit(st_pluginctx_t *plugin_ctx);

static bool st_plugin_load(st_pluginctx_t *plugin_ctx, const char *filename,
 bool force);
static bool st_plugin_memload(st_pluginctx_t *plugin_ctx, const void *data,
 size_t size, bool force);
