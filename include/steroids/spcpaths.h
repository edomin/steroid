#ifndef ST_STEROIDS_SPCPATHS_H
#define ST_STEROIDS_SPCPATHS_H

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/spcpaths.h"

static st_modctx_t *st_spcpaths_init(st_modctx_t *logger_ctx);
static void st_spcpaths_quit(st_modctx_t *spcpaths_ctx);
static void st_spcpaths_get_config_path(st_modctx_t *spcpaths_ctx, char *dst,
 size_t dstlen, const char *appname);
static void st_spcpaths_get_data_path(st_modctx_t *spcpaths_ctx, char *dst,
 size_t dstlen, const char *appname);
static void st_spcpaths_get_cache_path(st_modctx_t *spcpaths_ctx, char *dst,
 size_t dstlen, const char *appname);

#endif /* ST_STEROIDS_SPCPATHS_H */
