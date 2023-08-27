#ifndef ST_STEROIDS_TYPES_MODULES_WINDOW_H
#define ST_STEROIDS_TYPES_MODULES_WINDOW_H

#include "steroids/module.h"
#include "steroids/types/modules/monitor.h"

#ifndef ST_WINDOW_T_DEFINED
    typedef struct st_window_s st_window_t;
#endif

typedef struct {
    st_window_t *window;
} st_evwinnoargs_t;

typedef struct {
    st_window_t *window;
    unsigned     width;
    unsigned     height;
} st_evwinresize_t;

// typedef struct {
//     st_window_t *window;
//     int          x;
//     int          y;
// } st_evwinmove_t;

typedef st_modctx_t *(*st_window_init_t)(st_modctx_t *events_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *monitor_ctx);
typedef void (*st_window_quit_t)(st_modctx_t *window_ctx);

typedef st_window_t *(*st_window_create_t)(st_modctx_t *window_ctx,
 st_monitor_t *monitor, int x, int y, unsigned width, unsigned height,
 bool fullscreen, const char *title);
typedef void (*st_window_destroy_t)(st_window_t *window);
typedef void (*st_window_process_t)(st_modctx_t *window_ctx);
typedef bool (*st_window_xed_t)(const st_window_t *window);
typedef st_modctx_t *(*st_window_get_ctx_t)(st_window_t *window);
typedef st_monitor_t *(*st_window_get_monitor_t)(st_window_t *window);
typedef void *(*st_window_get_handle_t)(st_window_t *window);
typedef unsigned (*st_window_get_width_t)(const st_window_t *window);
typedef unsigned (*st_window_get_height_t)(const st_window_t *window);

typedef struct {
    st_window_init_t        window_init;
    st_window_quit_t        window_quit;
    st_window_create_t      window_create;
    st_window_destroy_t     window_destroy;
    st_window_process_t     window_process;
    st_window_xed_t         window_xed;
    st_window_get_ctx_t     window_get_ctx;
    st_window_get_monitor_t window_get_monitor;
    st_window_get_handle_t  window_get_handle;
    st_window_get_width_t   window_get_width;
    st_window_get_height_t  window_get_height;
} st_window_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_WINDOW_H */
