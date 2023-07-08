#ifndef ST_STEROIDS_PATHTOOLS_H
#define ST_STEROIDS_PATHTOOLS_H

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/pathtools.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_pathtools_init(st_modctx_t *logger_ctx);
ST_VISIBILITY void st_pathtools_quit(st_modctx_t *pathtools_ctx);
ST_VISIBILITY bool st_pathtools_concat(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path, const char *append);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_PATHTOOLS_H */
