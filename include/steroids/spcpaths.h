#pragma once

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/spcpaths.h"

static st_spcpathsctx_t *st_spcpaths_init(struct st_loggerctx_s *logger_ctx);
static void st_spcpaths_quit(st_spcpathsctx_t *spcpaths_ctx);
static void st_spcpaths_get_config_path(st_spcpathsctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);
static void st_spcpaths_get_data_path(st_spcpathsctx_t *spcpaths_ctx, char *dst,
 size_t dstlen, const char *appname);
static void st_spcpaths_get_cache_path(st_spcpathsctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);
