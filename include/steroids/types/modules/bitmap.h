#ifndef ST_STEROIDS_TYPES_MODULES_BITMAP_H
#define ST_STEROIDS_TYPES_MODULES_BITMAP_H

#include "steroids/module.h"

#ifndef ST_BITMAP_T_DEFINED
    typedef struct st_bitmap_s st_bitmap_t;
#endif

typedef enum {
    PF_UNKNOWN,
    PF_RGBA,
} st_pxfmt_t;

typedef st_modctx_t *(*st_bitmap_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_bitmap_quit_t)(st_modctx_t *bitmap_ctx);

typedef st_bitmap_t *(*st_bitmap_import_t)(st_modctx_t *bitmap_ctx,
 const void *data, unsigned width, unsigned height, st_pxfmt_t pixel_format);
typedef void (*st_bitmap_destroy_t)(st_bitmap_t *bitmap);
typedef void *(*st_bitmap_get_data_t)(st_bitmap_t *bitmap);
typedef unsigned (*st_bitmap_get_width_t)(st_bitmap_t *bitmap);
typedef unsigned (*st_bitmap_get_height_t)(st_bitmap_t *bitmap);
typedef st_pxfmt_t (*st_bitmap_get_pixel_format_t)(st_bitmap_t *bitmap);

typedef struct {
    st_bitmap_init_t             bitmap_init;
    st_bitmap_quit_t             bitmap_quit;
    st_bitmap_import_t           bitmap_import;
    st_bitmap_destroy_t          bitmap_destroy;
    st_bitmap_get_data_t         bitmap_get_data;
    st_bitmap_get_width_t        bitmap_get_width;
    st_bitmap_get_height_t       bitmap_get_height;
    st_bitmap_get_pixel_format_t bitmap_get_pixel_format;
} st_bitmap_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_BITMAP_H */
