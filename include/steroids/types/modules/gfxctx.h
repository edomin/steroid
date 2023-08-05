#ifndef ST_STEROIDS_TYPES_MODULES_GFXCTX_H
#define ST_STEROIDS_TYPES_MODULES_GFXCTX_H

#include "steroids/module.h"
#include "steroids/types/modules/monitor.h"
#include "steroids/types/modules/window.h"

#ifndef ST_GFXCTX_T_DEFINED
    typedef struct st_gfxctx_s st_gfxctx_t;
#endif

typedef enum {
    ST_GAPI_GL11,
    ST_GAPI_GL12,
    ST_GAPI_GL13,
    ST_GAPI_GL14,
    ST_GAPI_GL15,
    ST_GAPI_GL2,
    ST_GAPI_GL21,
    ST_GAPI_GL3,
    ST_GAPI_GL31,
    ST_GAPI_GL32,
    ST_GAPI_GL33,
    ST_GAPI_GL4,
    ST_GAPI_GL41,
    ST_GAPI_GL42,
    ST_GAPI_GL43,
    ST_GAPI_GL44,
    ST_GAPI_GL45,
    ST_GAPI_GL46,
    ST_GAPI_ES1,
    ST_GAPI_ES11,
    ST_GAPI_ES2,
    ST_GAPI_ES3,
    ST_GAPI_ES31,
    ST_GAPI_ES32,

    ST_GAPI_MAX,
    ST_GAPI_UNKNOWN = ST_GAPI_MAX,
} st_gapi_t;

typedef st_modctx_t *(*st_gfxctx_init_t)(st_modctx_t *logger_ctx,
 st_modctx_t *monitor_ctx, st_modctx_t *window_ctx);
typedef void (*st_gfxctx_quit_t)(st_modctx_t *gfxctx_ctx);

typedef st_gfxctx_t *(*st_gfxctx_create_t)(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, st_gapi_t api);
typedef st_gfxctx_t *(*st_gfxctx_create_shared_t)(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, st_gfxctx_t *other);
typedef bool (*st_gfxctx_make_current_t)(st_gfxctx_t *gfxctx);
typedef bool (*st_gfxctx_swap_buffers_t)(st_gfxctx_t *gfxctx);
typedef st_gapi_t (*st_gfxctx_get_api_t)(st_gfxctx_t *gfxctx);
typedef void (*st_gfxctx_destroy_t)(st_gfxctx_t *gfxctx);

typedef struct {
    st_gfxctx_init_t          gfxctx_init;
    st_gfxctx_quit_t          gfxctx_quit;
    st_gfxctx_create_t        gfxctx_create;
    st_gfxctx_create_shared_t gfxctx_create_shared;
    st_gfxctx_make_current_t  gfxctx_make_current;
    st_gfxctx_swap_buffers_t  gfxctx_swap_buffers;
    st_gfxctx_get_api_t       gfxctx_get_api;
    st_gfxctx_destroy_t       gfxctx_destroy;
} st_gfxctx_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_GFXCTX_H */
