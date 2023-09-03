#ifndef ST_MODULES_TEXTURE_OPENGL_TYPES_H
#define ST_MODULES_TEXTURE_OPENGL_TYPES_H

#include <GL/gl.h>

#include "steroids/types/modules/bitmap.h"
#include "steroids/types/modules/gfxctx.h"
#include "steroids/types/modules/glloader.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_glloader_get_proc_address_t get_proc_address;
} st_texture_opengl_glloader_t;

typedef struct {
    st_modctx_t           *ctx;
    st_bitmap_load_t       load;
    st_bitmap_memload_t    memload;
    st_bitmap_destroy_t    destroy;
    st_bitmap_get_data_t   get_data;
    st_bitmap_get_width_t  get_width;
    st_bitmap_get_height_t get_height;
} st_texture_opengl_bitmap_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_texture_opengl_logger_t;

typedef struct {
    st_texture_opengl_bitmap_t   bitmap;
    st_texture_opengl_glloader_t glloader;
    st_texture_opengl_logger_t   logger;
    st_gapi_t                    api;
} st_texture_opengl_t;

typedef struct {
    st_texture_opengl_t *module;
    GLuint               id;
    unsigned             width;
    unsigned             height;
} st_texture_t;

#define ST_TEXTURE_T_DEFINED

#endif /* ST_MODULES_TEXTURE_OPENGL_TYPES_H */
