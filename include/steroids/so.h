#ifndef ST_STEROIDS_SO_H
#define ST_STEROIDS_SO_H

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/so.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_so_init(st_modctx_t *logger_ctx);
ST_VISIBILITY void st_so_quit(st_modctx_t *so_ctx);
ST_VISIBILITY bool st_so_load_module(st_modctx_t *so_ctx, const char *filename);
ST_VISIBILITY bool st_so_memload_module(st_modctx_t *so_ctx, const void *data,
 size_t size);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_SO_H */
