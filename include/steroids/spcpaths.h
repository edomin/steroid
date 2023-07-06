#ifndef ST_STEROIDS_SPCPATHS_H
#define ST_STEROIDS_SPCPATHS_H

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/spcpaths.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_spcpaths_init(st_modctx_t *logger_ctx);
ST_VISIBILITY void st_spcpaths_quit(st_modctx_t *spcpaths_ctx);
ST_VISIBILITY void st_spcpaths_get_config_path(st_modctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);
ST_VISIBILITY void st_spcpaths_get_data_path(st_modctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);
ST_VISIBILITY void st_spcpaths_get_cache_path(st_modctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_SPCPATHS_H */
