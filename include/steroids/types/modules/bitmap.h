#ifndef ST_STEROIDS_TYPES_MODULES_BITMAP_H
#define ST_STEROIDS_TYPES_MODULES_BITMAP_H

#include "steroids/module.h"

#ifndef ST_BITMAP_T_DEFINED
    typedef struct st_bitmap_s st_bitmap_t;
#endif

typedef enum {
    PF_UNKNOWN,
    PF_RGBA,

    PF_MAX,
} st_pxfmt_t;

#ifndef ST_BITMAP_CODEC_FUNCS_DEFINED
    typedef st_bitmap_t *(*st_bmcodec_load_t)(st_modctx_t *codec_ctx,
     const char *filename);
    typedef st_bitmap_t *(*st_bmcodec_memload_t)(st_modctx_t *codec_ctx,
     const void *data, size_t size);
    typedef bool (*st_bmcodec_save_t)(st_modctx_t *codec_ctx,
     const st_bitmap_t *bitmap, const char *filename);
    typedef const char *(*st_bmcodec_memsave_t)(st_modctx_t *codec_ctx,
     void *dst, size_t *size, const st_bitmap_t *bitmap);
#endif

typedef st_modctx_t *(*st_bitmap_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_bitmap_quit_t)(st_modctx_t *bitmap_ctx);

typedef void (*st_bitmap_add_codec_t)(st_modctx_t *bitmap_ctx,
 st_modctx_t *codec_ctx);
typedef st_bitmap_t *(*st_bitmap_load_t)(st_modctx_t *bitmap_ctx,
 const char *filename);
typedef st_bitmap_t *(*st_bitmap_memload_t)(st_modctx_t *bitmap_ctx,
 const void *data, size_t size);
typedef bool (*st_bitmap_save_t)(const st_bitmap_t *bitmap,
 const char *filename);
typedef const char *(*st_bitmap_memsave_t)(void *dst, size_t *size,
 const st_bitmap_t *bitmap);
typedef st_bitmap_t *(*st_bitmap_import_t)(st_modctx_t *bitmap_ctx,
 const void *data, unsigned width, unsigned height, st_pxfmt_t pixel_format);
typedef void (*st_bitmap_destroy_t)(st_bitmap_t *bitmap);
typedef const void *(*st_bitmap_get_data_t)(const st_bitmap_t *bitmap);
typedef unsigned (*st_bitmap_get_width_t)(const st_bitmap_t *bitmap);
typedef unsigned (*st_bitmap_get_height_t)(const st_bitmap_t *bitmap);
typedef st_pxfmt_t (*st_bitmap_get_pixel_format_t)(const st_bitmap_t *bitmap);

typedef struct {
    st_bitmap_init_t             bitmap_init;
    st_bitmap_quit_t             bitmap_quit;
    st_bitmap_add_codec_t        bitmap_add_codec;
    st_bitmap_load_t             bitmap_load;
    st_bitmap_memload_t          bitmap_memload;
    st_bitmap_save_t             bitmap_save;
    st_bitmap_memsave_t          bitmap_memsave;
    st_bitmap_import_t           bitmap_import;
    st_bitmap_destroy_t          bitmap_destroy;
    st_bitmap_get_data_t         bitmap_get_data;
    st_bitmap_get_width_t        bitmap_get_width;
    st_bitmap_get_height_t       bitmap_get_height;
    st_bitmap_get_pixel_format_t bitmap_get_pixel_format;
} st_bitmap_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_BITMAP_H */
