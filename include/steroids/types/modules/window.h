#ifndef ST_STEROIDS_TYPES_MODULES_WINDOW_H
#define ST_STEROIDS_TYPES_MODULES_WINDOW_H

#include "steroids/module.h"

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

typedef struct {
    st_window_init_t    window_init;
    st_window_quit_t    window_quit;
    st_window_create_t  window_create;
    st_window_destroy_t window_destroy;
    st_window_process_t window_process;
} st_window_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_WINDOW_H */
