#pragma once

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/pathtools.h"

static st_pathtoolsctx_t *st_pathtools_init(struct st_loggerctx_s *logger_ctx);
static void st_pathtools_quit(st_pathtoolsctx_t *pathtools_ctx);

static bool st_pathtools_resolve(st_pathtoolsctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path);
static bool st_pathtools_get_parent_dir(st_pathtoolsctx_t *pathtools_ctx,
 char *dst, size_t dstsize, const char *path);
static bool st_pathtools_concat(st_pathtoolsctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path, const char *append);
