#pragma once

#include <stddef.h>

#include "steroids/module.h"

typedef enum {
    ST_PT_IRI,
    ST_PT_ABSOLUTE,
    ST_PT_RELATIVE,
    ST_PT_NOT_PATH,
} st_pathtype_t;

typedef st_modctx_t *(*st_pathtools_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_pathtools_quit_t)(st_modctx_t *pathtools_ctx);

typedef bool (*st_pathtools_resolve_t)(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path);
typedef bool (*st_pathtools_get_parent_dir_t)(st_modctx_t *pathtools_ctx,
 char *dst, size_t dstsize, const char *path);
typedef bool (*st_pathtools_concat_t)(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path, const char *append);

typedef struct {
    st_pathtools_init_t           pathtools_init;
    st_pathtools_quit_t           pathtools_quit;
    st_pathtools_resolve_t        pathtools_resolve;
    st_pathtools_get_parent_dir_t pathtools_get_parent_dir;
    st_pathtools_concat_t         pathtools_concat;
} st_pathtools_funcs_t;
