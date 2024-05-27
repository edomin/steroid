#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"

#ifndef ST_DYNARR_T_DEFINED
    typedef struct st_dynarr_s st_dynarr_t;
#endif

typedef st_modctx_t *(*st_dynarr_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_dynarr_quit_t)(st_modctx_t *dynarr_ctx);

typedef st_dynarr_t *(*st_dynarr_create_t)(st_modctx_t *dynarr_ctx,
 size_t data_size, size_t initial_capacity);
typedef void (*st_dynarr_destroy_t)(st_dynarr_t *dynarr);
typedef bool (*st_dynarr_append_t)(st_dynarr_t *dynarr, const void *data);
typedef bool (*st_dynarr_set_t)(st_dynarr_t *dynarr, size_t index,
 const void *data);
typedef bool (*st_dynarr_clear_t)(st_dynarr_t *dynarr);
typedef bool (*st_dynarr_sort_t)(st_dynarr_t *dynarr,
 int (*cmpfunc)(const void *, const void *, void *), void *userptr);
typedef bool (*st_dynarr_extract_t)(const st_dynarr_t *dynarr, void *dst,
 size_t index);
typedef const void *(*st_dynarr_get_t)(st_dynarr_t *dynarr, size_t index);
typedef const void *(*st_dynarr_get_all_t)(st_dynarr_t *dynarr);
typedef size_t (*st_dynarr_get_elements_count_t)(const st_dynarr_t *dynarr);
typedef bool (*st_dynarr_is_empty_t)(const st_dynarr_t *dynarr);

typedef struct {
    st_dynarr_init_t               dynarr_init;
    st_dynarr_quit_t               dynarr_quit;
    st_dynarr_create_t             dynarr_create;
    st_dynarr_destroy_t            dynarr_destroy;
    st_dynarr_append_t             dynarr_append;
    st_dynarr_set_t                dynarr_set;
    st_dynarr_clear_t              dynarr_clear;
    st_dynarr_sort_t               dynarr_sort;
    st_dynarr_extract_t            dynarr_extract;
    st_dynarr_get_t                dynarr_get;
    st_dynarr_get_all_t            dynarr_get_all;
    st_dynarr_get_elements_count_t dynarr_get_elements_count;
    st_dynarr_is_empty_t           dynarr_is_empty;
} st_dynarr_funcs_t;
