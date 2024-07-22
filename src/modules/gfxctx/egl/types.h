#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "steroids/types/modules/fnv1a.h"
#include "steroids/types/modules/htable.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/monitor.h"
#include "steroids/types/modules/window.h"
#include "steroids/types/object.h"

#include "dlist.h"

typedef struct {
    st_modctx_t        *ctx;
    st_logger_debug_t   debug;
    st_logger_info_t    info;
    st_logger_warning_t warning;
    st_logger_error_t   error;
} st_gfxctx_egl_logger_t;

typedef struct {
    st_modctx_t            *ctx;
    st_monitor_get_handle_t get_handle;
} st_gfxctx_egl_monitor_t;

typedef struct {
    st_modctx_t            *ctx;
    st_window_get_handle_t  get_handle;
} st_gfxctx_egl_window_t;

typedef struct {
    st_fnv1a_get_u32hashstr_func_t get_u32hashstr_func;
} st_gfxctx_egl_fnv1a_t;

typedef struct {
    st_modctx_t        *ctx;
    st_htable_init_t    init;
    st_htable_quit_t    quit;
    st_htable_create_t  create;
    st_htable_destroy_t destroy;
    st_htable_insert_t  insert;
    st_htable_find_t    find;
} st_gfxctx_egl_htable_t;

typedef struct {
    st_gfxctx_egl_logger_t  logger;
    st_gfxctx_egl_monitor_t monitor;
    st_gfxctx_egl_window_t  window;
    st_gfxctx_egl_fnv1a_t   fnv1a;
    st_gfxctx_egl_htable_t  htable;
    bool                    debug_enabled;
    EGLint                (*egl_debug_message_control_khr)(
     EGLDEBUGPROCKHR callback, const EGLAttrib *attrib_list);
    EGLint                (*egl_label_object_khr)(EGLDisplay display,
     EGLenum objectType, EGLObjectKHR object, EGLLabelKHR label);
    bool                    must_quit;
    size_t                  gfxctxs_count;
} st_gfxctx_egl_t;

typedef struct {
    struct st_gfxctx_s *ctx;
    unsigned            index;
} st_gfxctx_shared_data_t;

ST_STRUCT_CLASS(st_gfxctx_s,
    st_window_t *window;
    EGLDisplay   display;
    EGLConfig    cfg;
    EGLSurface   surface;
    EGLContext   handle;
    int          gapi;
    bool         debug;
    st_dlist_t  *shared_data;
    st_htable_t *userdata;
) st_gfxctx_t;

#define ST_GFXCTX_T_DEFINED
