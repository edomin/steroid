#include "egl.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define RED_BITS         8
#define GREEN_BITS       8
#define BLUE_BITS        8
#define ALPHA_BITS       8

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_gfxctx_egl_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_gfxctx_egl_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_gfxctx_egl_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"gfxctx_egl\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_gfxctx_egl_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_gfxctx_egl_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_gfxctx_import_functions(st_modctx_t *gfxctx_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *monitor_ctx, st_modctx_t *window_ctx) {
    st_gfxctx_egl_t *module = gfxctx_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "gfxctx_egl: Unable to load function \"error\" from module "
         "\"logger\"\n"
        );

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("gfxctx_egl", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("gfxctx_egl", logger, info);
    ST_LOAD_FUNCTION_FROM_CTX("gfxctx_egl", logger, warning);

    ST_LOAD_FUNCTION_FROM_CTX("gfxctx_egl", monitor, get_handle);

    ST_LOAD_FUNCTION_FROM_CTX("gfxctx_egl", window, get_handle);

    return true;
}

static st_modctx_t *st_gfxctx_init(st_modctx_t *logger_ctx,
 st_modctx_t *monitor_ctx, st_modctx_t *window_ctx) {
    st_modctx_t     *gfxctx_ctx;
    st_gfxctx_egl_t *module;

    gfxctx_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_gfxctx_egl_data, sizeof(st_gfxctx_egl_t));

    if (!gfxctx_ctx)
        return NULL;

    gfxctx_ctx->funcs = &st_gfxctx_egl_funcs;

    module = gfxctx_ctx->data;
    module->logger.ctx = logger_ctx;
    module->monitor.ctx = monitor_ctx;
    module->window.ctx = window_ctx;

    if (!st_gfxctx_import_functions(gfxctx_ctx, logger_ctx, monitor_ctx,
     window_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, gfxctx_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx,
     "gfxctx_egl: Graphics context initialized");

    return gfxctx_ctx;
}

static void st_gfxctx_quit(st_modctx_t *gfxctx_ctx) {
    st_gfxctx_egl_t *module = gfxctx_ctx->data;

    module->logger.info(module->logger.ctx,
     "gfxctx_egl: Graphics context destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, gfxctx_ctx);
}

static EGLint get_renderable_type_by_gapi(st_gapi_t api) {
    if (api >= ST_GAPI_GL11 && api <= ST_GAPI_GL46)
        return EGL_OPENGL_BIT;
    if (api == ST_GAPI_ES1 || api == ST_GAPI_ES11)
        return EGL_OPENGL_ES_BIT;
    if (api == ST_GAPI_ES2)
        return EGL_OPENGL_ES2_BIT;
    if (api >= ST_GAPI_ES3 && api < ST_GAPI_ES32)
        return EGL_OPENGL_ES3_BIT_KHR;

    return (EGLint)-1;
}

static EGLint get_major_version_by_gapi(st_gapi_t api) {
    if ((api >= ST_GAPI_GL11 && api <= ST_GAPI_GL15) ||
     api == ST_GAPI_ES1 || api == ST_GAPI_ES11)
        return 1;
    if (api == ST_GAPI_GL2 || api == ST_GAPI_GL21 || api == ST_GAPI_ES2)
        return 2;
    if ((api >= ST_GAPI_GL3 && api <= ST_GAPI_GL33) ||
     (api >= ST_GAPI_ES3 && api <= ST_GAPI_ES32))
        return 3;
    if (api >= ST_GAPI_GL4 && api <= ST_GAPI_GL46)
        return 4;

    return (EGLint)-1;
}

static EGLint get_minor_version_by_gapi(st_gapi_t api) {
    switch (api) {
        case ST_GAPI_GL2:
        case ST_GAPI_GL3:
        case ST_GAPI_GL4:
        case ST_GAPI_ES1:
        case ST_GAPI_ES2:
        case ST_GAPI_ES3:
            return 0;
        case ST_GAPI_GL11:
        case ST_GAPI_GL21:
        case ST_GAPI_GL31:
        case ST_GAPI_GL41:
        case ST_GAPI_ES11:
        case ST_GAPI_ES31:
            return 1;
        case ST_GAPI_GL12:
        case ST_GAPI_GL32:
        case ST_GAPI_GL42:
        case ST_GAPI_ES32:
            return 2;
        case ST_GAPI_GL13:
        case ST_GAPI_GL33:
        case ST_GAPI_GL43:
            return 3;
        case ST_GAPI_GL14:
        case ST_GAPI_GL44:
            return 4;
        case ST_GAPI_GL15:
        case ST_GAPI_GL45:
            return 5; // NOLINT(readability-magic-numbers)
        case ST_GAPI_GL46:
            return 6; // NOLINT(readability-magic-numbers)
        case ST_GAPI_MAX:
        default:
            break;
    }

    return (EGLint)-1;
}

const char *get_egl_error_str(EGLint errorcode) {
    switch (errorcode) {
        case EGL_SUCCESS:
            return "The last function succeeded without error";
        case EGL_NOT_INITIALIZED:
            return "EGL is not initialized, or could not be initialized, for "
                   "the specified EGL display connection";
        case EGL_BAD_ACCESS:
            return "EGL cannot access a requested resource (for example a "
                   "context is bound in another thread)";
        case EGL_BAD_ALLOC:
            return "EGL failed to allocate resources for the requested "
                   "operation";
        case EGL_BAD_ATTRIBUTE:
            return "An unrecognized attribute or attribute value was passed in "
                   "the attribute list";
        case EGL_BAD_CONTEXT:
            return "An EGLContext argument does not name a valid EGL rendering "
                   "context";
        case EGL_BAD_CONFIG:
            return "An EGLConfig argument does not name a valid EGL frame "
                   "buffer configuration";
        case EGL_BAD_CURRENT_SURFACE:
            return "The current surface of the calling thread is a window, "
                   "pixel buffer or pixmap that is no longer valid";
        case EGL_BAD_DISPLAY:
            return "An EGLDisplay argument does not name a valid EGL display "
                   "connection";
        case EGL_BAD_SURFACE:
            return "An EGLSurface argument does not name a valid surface "
                   "(window, pixel buffer or pixmap) configured for GL "
                   "rendering";
        case EGL_BAD_MATCH:
            return "Arguments are inconsistent (for example, a valid context "
                   "requires buffers not supplied by a valid surface)";
        case EGL_BAD_PARAMETER:
            return "One or more argument values are invalid";
        case EGL_BAD_NATIVE_PIXMAP:
            return "A NativePixmapType argument does not refer to a valid "
                   "native pixmap";
        case EGL_BAD_NATIVE_WINDOW:
            return "A NativeWindowType argument does not refer to a valid "
                   "native window";
        case EGL_CONTEXT_LOST:
            return "A power management event has occurred. The application "
                   "must destroy all contexts and reinitialise OpenGL ES state "
                   "and objects to continue rendering";
        default:
            break;
    }

    return "Unknown error";
}

static st_gfxctx_t *st_gfxctx_create_impl(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, EGLint renderable_type,
 EGLint major, EGLint minor, EGLContext shared_context) {
    st_gfxctx_egl_t *module = gfxctx_ctx->data;
    EGLint           egl_cfg_attr[] = {
        EGL_RED_SIZE,        RED_BITS,
        EGL_GREEN_SIZE,      GREEN_BITS,
        EGL_BLUE_SIZE,       BLUE_BITS,
        EGL_ALPHA_SIZE,      ALPHA_BITS,
        EGL_RENDERABLE_TYPE, renderable_type,
        EGL_CONFIG_CAVEAT,   EGL_NONE,
        EGL_NONE,
    };
    EGLint           egl_ctx_attr[] = {
        EGL_CONTEXT_MAJOR_VERSION_KHR,       major,
        EGL_CONTEXT_MINOR_VERSION_KHR,       minor,
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
        EGL_NONE,
    };
    EGLint           configs_count;
    st_gfxctx_t     *gfxctx;

    gfxctx = malloc(sizeof(st_gfxctx_t));
    if (!gfxctx) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to allocate memory for gfx context: %s",
         strerror(errno));

        return NULL;
    }

    gfxctx->display = eglGetDisplay(
     (EGLNativeDisplayType)module->monitor.get_handle(monitor));

    if (gfxctx->display == EGL_NO_DISPLAY) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to get EGL display");

        goto get_display_fail;
    }

    if (eglInitialize(gfxctx->display, NULL, NULL) == EGL_FALSE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to initialize EGL: %s",
         get_egl_error_str(eglGetError()));

        goto egl_init_fail;
    }

    if (eglChooseConfig(gfxctx->display, egl_cfg_attr, &gfxctx->cfg, 1,
     &configs_count) == EGL_FALSE || configs_count != 1) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to get matching frame buffer configuration: %s",
         get_egl_error_str(eglGetError()));

        goto choose_config_fail;
    }

    gfxctx->surface = eglCreatePlatformWindowSurface(gfxctx->display,
     gfxctx->cfg, module->window.get_handle(window), NULL);
    if (gfxctx->surface == EGL_NO_SURFACE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to create EGL window surface: %s",
         get_egl_error_str(eglGetError()));

        goto create_surface_fail;
    }

    // TODO(edomin): share data with other context
    gfxctx->handle = eglCreateContext(gfxctx->display, gfxctx->cfg,
     shared_context, egl_ctx_attr);

    if (gfxctx->handle == EGL_NO_CONTEXT) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to create EGL render context");

        goto create_context_fail;
    }

    eglMakeCurrent(gfxctx->display, gfxctx->surface, gfxctx->surface,
     gfxctx->handle);

    if (eglSwapInterval(gfxctx->display, 1) == EGL_FALSE)
        module->logger.warning(module->logger.ctx,
         "gfxctx_egl: Unable to set swap interval: %s",
         get_egl_error_str(eglGetError()));

    return gfxctx;

create_context_fail:
    eglDestroySurface(gfxctx->display, gfxctx->surface);
create_surface_fail:
choose_config_fail:
    eglTerminate(gfxctx->display);
egl_init_fail:
get_display_fail:
    free(gfxctx);

    return NULL;
}

static st_gfxctx_t *st_gfxctx_create(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, st_gapi_t api) {
    st_gfxctx_egl_t *module = gfxctx_ctx->data;

    if (api < ST_GAPI_GL11 || api >= ST_GAPI_MAX) {
        module->logger.error(module->logger.ctx, "gfxctx_egl: Unknown gfx API");

        return NULL;
    }

    return st_gfxctx_create_impl(gfxctx_ctx, monitor, window,
     get_renderable_type_by_gapi(api), get_major_version_by_gapi(api),
     get_minor_version_by_gapi(api), EGL_NO_CONTEXT);
}

static st_gfxctx_t *st_gfxctx_create_shared(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, st_gfxctx_t *other) {
    st_gfxctx_egl_t *module = gfxctx_ctx->data;
    EGLint           renderable_type;
    EGLint           major;
    EGLint           minor;

    if (eglGetConfigAttrib(other->display, other->cfg, EGL_RENDERABLE_TYPE,
     &renderable_type) == EGL_FALSE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to get renderable type of gfx context: %s",
         get_egl_error_str(eglGetError()));

        return NULL;
    }

    if (eglGetConfigAttrib(other->display, other->cfg,
     EGL_CONTEXT_MAJOR_VERSION_KHR, &major) == EGL_FALSE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to get major version of gfx context: %s",
         get_egl_error_str(eglGetError()));

        return NULL;
    }

    if (eglGetConfigAttrib(other->display, other->cfg,
     EGL_CONTEXT_MINOR_VERSION_KHR, &minor) == EGL_FALSE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to get minor version of gfx context: %s",
         get_egl_error_str(eglGetError()));

        return NULL;
    }

    return st_gfxctx_create_impl(gfxctx_ctx, monitor, window, renderable_type,
     major, minor, other->handle);
}

static bool st_gfxctx_make_current(st_gfxctx_t *gfxctx) {
    return eglMakeCurrent(gfxctx->display, gfxctx->surface, gfxctx->surface,
     gfxctx->handle);
}

static bool st_gfxctx_swap_buffers(st_gfxctx_t *gfxctx) {
    return eglSwapBuffers(gfxctx->display, gfxctx->surface);
}

static void st_gfxctx_destroy(st_gfxctx_t *gfxctx) {
    if (eglGetCurrentContext() == gfxctx->handle)
        eglMakeCurrent(gfxctx->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
         EGL_NO_CONTEXT);
    eglDestroyContext(gfxctx->display, gfxctx->handle);
    eglDestroySurface(gfxctx->display, gfxctx->surface);
    eglTerminate(gfxctx->display);
    free(gfxctx);
}
