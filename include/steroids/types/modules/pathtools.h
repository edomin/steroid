#ifndef ST_STEROIDS_TYPES_MODULES_PATHTOOLS_H
#define ST_STEROIDS_TYPES_MODULES_PATHTOOLS_H

#include <stddef.h>

#include "steroids/module.h"

typedef st_modctx_t *(*st_pathtools_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_pathtools_quit_t)(st_modctx_t *pathtools_ctx);
typedef bool (*st_pathtools_concat_t)(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path, const char *append);

typedef struct {
    st_pathtools_init_t   pathtools_init;
    st_pathtools_quit_t   pathtools_quit;
    st_pathtools_concat_t pathtools_concat;
} st_pathtools_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_PATHTOOLS_H */
