#ifndef ST_STEROIDS_PATHTOOLS_H
#define ST_STEROIDS_PATHTOOLS_H

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/pathtools.h"

static st_modctx_t *st_pathtools_init(st_modctx_t *logger_ctx);
static void st_pathtools_quit(st_modctx_t *pathtools_ctx);

static bool st_pathtools_resolve(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path);
static bool st_pathtools_get_parent_dir(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path);
static bool st_pathtools_concat(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path, const char *append);

#endif /* ST_STEROIDS_PATHTOOLS_H */
