#pragma once

#include <stdbool.h>

#include "steroids/types/modules/dynarr.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/sprite.h"
#include "steroids/types/object.h"

typedef struct {
    st_modctx_t                   *ctx;
    st_dynarr_create_t             create;
    st_dynarr_destroy_t            destroy;
    st_dynarr_append_t             append;
    st_dynarr_clear_t              clear;
    st_dynarr_sort_t               sort;
    st_dynarr_export_t             export;
    st_dynarr_get_all_t            get_all;
    st_dynarr_get_elements_count_t get_elements_count;
    st_dynarr_is_empty_t           is_empty;
} st_drawq_simple_dynarr_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_drawq_simple_logger_t;

typedef struct {
    st_modctx_t            *ctx;
    st_sprite_get_texture_t get_texture;
} st_drawq_simple_sprite_t;

typedef struct {
    st_drawq_simple_dynarr_t dynarr;
    st_drawq_simple_logger_t logger;
    st_drawq_simple_sprite_t sprite;
} st_drawq_simple_t;

ST_CLASS(
    st_dynarr_t *entries;
) st_drawq_t;

#define ST_DRAWQ_T_DEFINED
