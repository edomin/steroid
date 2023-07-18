#ifndef ST_STEROIDS_TYPES_MODULES_RBUF_H
#define ST_STEROIDS_TYPES_MODULES_RBUF_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"

#ifndef ST_RBUF_T_DEFINED
    typedef struct st_rbuf_s st_rbuf_t;
#endif

typedef st_modctx_t *(*st_rbuf_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_rbuf_quit_t)(st_modctx_t *rbuf_ctx);

typedef st_rbuf_t *(*st_rbuf_create_t)(st_modctx_t *rbuf_ctx, size_t size);
typedef void (*st_rbuf_destroy_t)(st_rbuf_t *rbuf);
typedef bool (*st_rbuf_push_t)(st_rbuf_t *rbuf, const void *data, size_t size);
typedef bool (*st_rbuf_peek_t)(const st_rbuf_t *rbuf, void *data, size_t size);
typedef bool (*st_rbuf_pop_t)(st_rbuf_t *rbuf, void *data, size_t size);
typedef bool (*st_rbuf_drop_t)(st_rbuf_t *rbuf, size_t size);
typedef bool (*st_rbuf_clear_t)(st_rbuf_t *rbuf);
typedef size_t (*st_rbuf_get_free_space_t)(const st_rbuf_t *rbuf);
typedef bool (*st_rbuf_is_empty_t)(const st_rbuf_t *rbuf);

typedef struct {
    st_rbuf_init_t           rbuf_init;
    st_rbuf_quit_t           rbuf_quit;
    st_rbuf_create_t         rbuf_create;
    st_rbuf_destroy_t        rbuf_destroy;
    st_rbuf_push_t           rbuf_push;
    st_rbuf_peek_t           rbuf_peek;
    st_rbuf_pop_t            rbuf_pop;
    st_rbuf_drop_t           rbuf_drop;
    st_rbuf_clear_t          rbuf_clear;
    st_rbuf_get_free_space_t rbuf_get_free_space;
    st_rbuf_is_empty_t       rbuf_is_empty;
} st_rbuf_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_RBUF_H */
