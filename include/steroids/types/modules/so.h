#ifndef ST_STEROIDS_TYPES_MODULES_SO_H
#define ST_STEROIDS_TYPES_MODULES_SO_H

#include <stddef.h>

#include "steroids/module.h"

typedef struct st_so st_so;

typedef st_modctx_t *(*st_so_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_so_quit_t)(st_modctx_t *so_ctx);
typedef st_so *(*st_so_open_t)(st_modctx_t *so_ctx, const char *filename);
typedef st_so *(*st_so_memopen_t)(st_modctx_t *so_ctx, const void *data,
 size_t size);
typedef void (*st_so_close_t)(st_modctx_t *so_ctx, st_so *so);
typedef void *(*st_so_load_symbol_t)(st_modctx_t *so_ctx, st_so *so,
 const char *name);

typedef struct {
    st_so_init_t        so_init;
    st_so_quit_t        so_quit;
    st_so_open_t        so_open;
    st_so_memopen_t     so_memopen;
    st_so_close_t       so_close;
    st_so_load_symbol_t so_load_symbol;
} st_so_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_SO_H */
