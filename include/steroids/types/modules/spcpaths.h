#ifndef ST_STEROIDS_TYPES_MODULES_SPCPATHS_H
#define ST_STEROIDS_TYPES_MODULES_SPCPATHS_H

#include <stddef.h>

#include "steroids/module.h"

typedef st_modctx_t *(*st_spcpaths_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_spcpaths_quit_t)(st_modctx_t *spcpaths_ctx);
typedef void (*st_spcpaths_get_config_path_t)(st_modctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);
typedef void (*st_spcpaths_get_data_path_t)(st_modctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);
typedef void (*st_spcpaths_get_cache_path_t)(st_modctx_t *spcpaths_ctx,
 char *dst, size_t dstlen, const char *appname);

typedef struct {
    st_spcpaths_init_t            spcpaths_init;
    st_spcpaths_quit_t            spcpaths_quit;
    st_spcpaths_get_config_path_t spcpaths_get_config_path;
    st_spcpaths_get_data_path_t   spcpaths_get_data_path;
    st_spcpaths_get_cache_path_t  spcpaths_get_cache_path;
} st_spcpaths_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_SPCPATHS_H */
