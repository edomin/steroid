#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/texture.h"
#include "steroids/types/object.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_atlas_simple_logger_t;

typedef struct {
    st_modctx_t *ctx;
} st_atlas_simple_texture_t;

typedef struct {
    st_atlas_simple_logger_t  logger;
    st_atlas_simple_texture_t texture;
} st_atlas_simple_t;

typedef struct {
    unsigned x;
    unsigned y;
    unsigned width;
    unsigned height;
} st_texclip_t;

ST_CLASS(
    st_texture_t *texture;
    size_t        clips_count;
    st_texclip_t  clips[];
) st_atlas_t;

#define ST_ATLAS_T_DEFINED
