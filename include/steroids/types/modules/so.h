#ifndef ST_STEROIDS_TYPES_MODULES_SO_H
#define ST_STEROIDS_TYPES_MODULES_SO_H

#include <stddef.h>

#include "steroids/module.h"

typedef st_modctx_t *(*st_so_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_so_quit_t)(st_modctx_t *so_ctx);
typedef bool (*st_so_load_module_t)(st_modctx_t *so_ctx, const char *filename);
typedef bool (*st_so_memload_module_t)(st_modctx_t *so_ctx, const void *data,
 size_t size);

typedef struct {
    st_so_init_t           so_init;
    st_so_quit_t           so_quit;
    st_so_load_module_t    so_load_module;
    st_so_memload_module_t so_memload_module;
} st_so_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_SO_H */
