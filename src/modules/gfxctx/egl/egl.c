#include "egl.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE               1024
#define GAPI_STR_SIZE_MAX                32
#define RED_BITS                          8
#define GREEN_BITS                        8
#define BLUE_BITS                         8
#define ALPHA_BITS                        8

#define RED_SIZE_INDEX                    0
#define GREEN_SIZE_INDEX                  2
#define BLUE_SIZE_INDEX                   4
#define ALPHA_SIZE_INDEX                  6
#define CONFIG_CAVEAT_INDEX               8
#define RENDERABLE_TYPE_INDEX            10
#define CFG_ATTR_END_INDEX               12
#define CFG_ATTRS_LEN                    13

#define CONTEXT_MAJOR_VERSION_INDEX       0
#define CONTEXT_MINOR_VERSION_INDEX       2
#define CONTEXT_OPENGL_PROFILE_MASK_INDEX 4
#define CTX_ATTR_END_INDEX                6
#define CTX_ATTRS_LEN                     7

typedef struct {
    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint alpha_size;
    EGLint renderable_type;
} cfg_attrs_t;

typedef struct {
    EGLint context_major_version;
    EGLint context_minor_version;
} ctx_attrs_t;

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(gfxctx_egl)
ST_MODULE_DEF_INIT_FUNC(gfxctx_egl)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
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
     "gfxctx_egl: Graphics context mgr initialized");

    return gfxctx_ctx;
}

static void st_gfxctx_quit(st_modctx_t *gfxctx_ctx) {
    st_gfxctx_egl_t *module = gfxctx_ctx->data;

    module->logger.info(module->logger.ctx,
     "gfxctx_egl: Graphics context mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, gfxctx_ctx);
}

static EGLenum getegl_api_by_gapi(st_gapi_t api) {
    if (api >= ST_GAPI_GL11 && api <= ST_GAPI_GL46)
        return EGL_OPENGL_API;
    if (api >= ST_GAPI_ES1 && api <= ST_GAPI_ES32)
        return  EGL_OPENGL_ES_API;

    return (EGLenum)-1;
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

static bool extension_supported(EGLDisplay display, const char *ext) {
    const char *extensions = eglQueryString(display, EGL_EXTENSIONS);

    return extensions && strstr(extensions, ext);
}

static bool process_attrs(cfg_attrs_t *cfg_arrts, ctx_attrs_t *ctx_attrs,
 EGLint egl_version_minor, bool have_ctx_extension) {
    bool attrs_changed = false;

    switch (egl_version_minor) {
        case 0:
        case 1:
            if (cfg_arrts->renderable_type != EGL_OPENGL_ES_BIT ||
             ctx_attrs->context_major_version != 1 ||
             ctx_attrs->context_minor_version != 0) {
                attrs_changed = true;
                cfg_arrts->renderable_type = EGL_OPENGL_ES_BIT;
                ctx_attrs->context_major_version = 1;
                ctx_attrs->context_minor_version = 0;
            }
            break;
        case 2:
            if (cfg_arrts->renderable_type == EGL_OPENGL_ES_BIT &&
             (ctx_attrs->context_major_version != 1 ||
             ctx_attrs->context_minor_version != 0)) {
                attrs_changed = true;
                ctx_attrs->context_major_version = 1;
                ctx_attrs->context_minor_version = 0;
            } else if (cfg_arrts->renderable_type != EGL_OPENGL_ES2_BIT) {
                attrs_changed = true;
                cfg_arrts->renderable_type = EGL_OPENGL_ES2_BIT;
                ctx_attrs->context_major_version = 2;
                ctx_attrs->context_minor_version = 0;
            }
            break;
        case 4:
            if ((cfg_arrts->renderable_type == EGL_OPENGL_ES_BIT ||
             cfg_arrts->renderable_type == EGL_OPENGL_BIT) &&
             (!have_ctx_extension && ctx_attrs->context_minor_version != 0)) {
                attrs_changed = true;
                ctx_attrs->context_minor_version = 0;
            } else if (cfg_arrts->renderable_type == EGL_OPENGL_ES3_BIT_KHR &&
             !have_ctx_extension) {
                cfg_arrts->renderable_type = EGL_OPENGL_ES2_BIT;
                ctx_attrs->context_major_version = 2;
                ctx_attrs->context_minor_version = 0;
            }
            break;
        case 5: // NOLINT(readability-magic-numbers)
            if (cfg_arrts->renderable_type == EGL_OPENGL_ES3_BIT_KHR)
                cfg_arrts->renderable_type = EGL_OPENGL_ES3_BIT;
        default:
            break;
    }

    return attrs_changed;
}

static void fill_cfg_attrs(EGLint dst[CFG_ATTRS_LEN], cfg_attrs_t *attrs,
 EGLint egl_version_minor) {
    dst[RED_SIZE_INDEX] = EGL_RED_SIZE;
    dst[RED_SIZE_INDEX + 1] = attrs->red_size;
    dst[GREEN_SIZE_INDEX] = EGL_GREEN_SIZE;
    dst[GREEN_SIZE_INDEX + 1] = attrs->green_size;
    dst[BLUE_SIZE_INDEX] = EGL_BLUE_SIZE;
    dst[BLUE_SIZE_INDEX + 1] = attrs->blue_size;
    dst[ALPHA_SIZE_INDEX] = EGL_ALPHA_SIZE;
    dst[ALPHA_SIZE_INDEX + 1] = attrs->alpha_size;
    dst[CONFIG_CAVEAT_INDEX] = EGL_CONFIG_CAVEAT;
    dst[CONFIG_CAVEAT_INDEX + 1] = EGL_NONE;
    dst[RENDERABLE_TYPE_INDEX] = EGL_RENDERABLE_TYPE;
    if (attrs->renderable_type == EGL_OPENGL_ES3_BIT_KHR) {
        if (egl_version_minor == 5) // NOLINT(readability-magic-numbers)
            dst[RENDERABLE_TYPE_INDEX + 1] = EGL_OPENGL_ES3_BIT;
    }
    dst[CFG_ATTR_END_INDEX] = EGL_NONE;
}

static void fill_ctx_attrs(EGLint dst[CTX_ATTRS_LEN], ctx_attrs_t *attrs,
 EGLint egl_version_minor, bool have_ctx_extension) {
    if (attrs->context_major_version == EGL_NONE) {
        dst[CONTEXT_MAJOR_VERSION_INDEX] = EGL_NONE;
    } else {
        if (egl_version_minor < 4 ||
         (egl_version_minor == 4 && !have_ctx_extension)) {
            dst[CONTEXT_MAJOR_VERSION_INDEX] = EGL_CONTEXT_CLIENT_VERSION;
            dst[CONTEXT_MAJOR_VERSION_INDEX + 1] = attrs->context_major_version;
            dst[CONTEXT_MINOR_VERSION_INDEX] = EGL_NONE;
        } else if (egl_version_minor == 4 && have_ctx_extension) {
            dst[CONTEXT_MAJOR_VERSION_INDEX] = EGL_CONTEXT_MAJOR_VERSION_KHR;
            dst[CONTEXT_MAJOR_VERSION_INDEX + 1] = attrs->context_major_version;
            dst[CONTEXT_MINOR_VERSION_INDEX] = EGL_CONTEXT_MINOR_VERSION_KHR;
            dst[CONTEXT_MINOR_VERSION_INDEX + 1] = attrs->context_minor_version;
            dst[CONTEXT_OPENGL_PROFILE_MASK_INDEX] =
             EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;
            dst[CONTEXT_OPENGL_PROFILE_MASK_INDEX + 1] =
             EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
            dst[CTX_ATTR_END_INDEX] = EGL_NONE;
        } else {
            dst[CONTEXT_MAJOR_VERSION_INDEX] = EGL_CONTEXT_MAJOR_VERSION;
            dst[CONTEXT_MAJOR_VERSION_INDEX + 1] = attrs->context_major_version;
            dst[CONTEXT_MINOR_VERSION_INDEX] = EGL_CONTEXT_MINOR_VERSION;
            dst[CONTEXT_MINOR_VERSION_INDEX + 1] = attrs->context_minor_version;
            dst[CONTEXT_OPENGL_PROFILE_MASK_INDEX] =
             EGL_CONTEXT_OPENGL_PROFILE_MASK;
            dst[CONTEXT_OPENGL_PROFILE_MASK_INDEX + 1] =
             EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
            dst[CTX_ATTR_END_INDEX] = EGL_NONE;
        }
    }
}

static void attrs_fill_gapi_str(char dst[GAPI_STR_SIZE_MAX],
 cfg_attrs_t *cfg_arrts, ctx_attrs_t *ctx_attrs) {
    const char *api_name;

    switch (cfg_arrts->renderable_type) {
        case EGL_OPENGL_ES_BIT:
        case EGL_OPENGL_ES2_BIT:
        case EGL_OPENGL_ES3_BIT:
        #if EGL_OPENGL_ES3_BIT_KHR != EGL_OPENGL_ES3_BIT
        case EGL_OPENGL_ES3_BIT_KHR:
        #endif
            api_name = "OpenGL ES";
            break;
        case EGL_OPENGL_BIT:
            api_name = "OpenGL";
            break;
        default:
            api_name = "Unknown";
            break;
    }

    sprintf_s(dst, GAPI_STR_SIZE_MAX, "%s %i.%i", api_name,
     ctx_attrs->context_major_version, ctx_attrs->context_minor_version);
}

static int gapi_from_attrs(cfg_attrs_t *cfg_arrts,
 ctx_attrs_t *ctx_attrs) {
    switch (cfg_arrts->renderable_type) {
        case EGL_OPENGL_ES_BIT:
        case EGL_OPENGL_ES2_BIT:
        case EGL_OPENGL_ES3_BIT:
        #if EGL_OPENGL_ES3_BIT_KHR != EGL_OPENGL_ES3_BIT
        case EGL_OPENGL_ES3_BIT_KHR:
        #endif
            switch (ctx_attrs->context_major_version) {
                case 1:
                    return (const int[]){
                        ST_GAPI_ES1,
                        ST_GAPI_ES11,
                    }[ctx_attrs->context_minor_version];
                case 2:
                    return ST_GAPI_ES2;
                case 3:
                    return (const int[]){
                        ST_GAPI_ES3,
                        ST_GAPI_ES31,
                        ST_GAPI_ES32,
                    }[ctx_attrs->context_minor_version];
                default:
                    return ST_GAPI_UNKNOWN;
            }
        case EGL_OPENGL_BIT:
            switch (ctx_attrs->context_major_version) {
                case 1:
                    return (const int[]){
                        ST_GAPI_UNKNOWN,
                        ST_GAPI_GL11,
                        ST_GAPI_GL12,
                        ST_GAPI_GL13,
                        ST_GAPI_GL14,
                        ST_GAPI_GL15,
                    }[ctx_attrs->context_minor_version];
                case 2:
                    return (const int[]){
                        ST_GAPI_GL2,
                        ST_GAPI_GL21,
                    }[ctx_attrs->context_minor_version];
                case 3:
                    return (const int[]){
                        ST_GAPI_GL3,
                        ST_GAPI_GL31,
                        ST_GAPI_GL32,
                        ST_GAPI_GL33,
                    }[ctx_attrs->context_minor_version];
                case 4:
                    return (const int[]){
                        ST_GAPI_GL4,
                        ST_GAPI_GL41,
                        ST_GAPI_GL42,
                        ST_GAPI_GL43,
                        ST_GAPI_GL44,
                        ST_GAPI_GL45,
                        ST_GAPI_GL46,
                    }[ctx_attrs->context_minor_version];
                default:
                    return ST_GAPI_UNKNOWN;
            }
        default:
            return ST_GAPI_UNKNOWN;
    }
}

static st_gfxctx_t *st_gfxctx_create_impl(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, EGLint renderable_type,
 EGLint major, EGLint minor, st_gfxctx_t *shared) {
    st_gfxctx_egl_t *module = gfxctx_ctx->data;
    cfg_attrs_t cfg_attrs = {
        .red_size        = RED_BITS,
        .green_size      = GREEN_BITS,
        .blue_size       = BLUE_BITS,
        .alpha_size      = ALPHA_BITS,
        .renderable_type = renderable_type,
    };
    ctx_attrs_t ctx_attrs = {
        .context_major_version = major,
        .context_minor_version = minor,
    };
    EGLint           egl_cfg_attrs[CFG_ATTRS_LEN];
    EGLint           egl_ctx_attrs[CTX_ATTRS_LEN];
    EGLint           configs_count;
    st_gfxctx_t     *gfxctx;
    bool             egl_khr_create_context_supported;
    EGLint           egl_version_major;
    EGLint           egl_version_minor;
    bool             attrs_changed = false;

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

    egl_khr_create_context_supported = extension_supported(gfxctx->display,
     "EGL_KHR_create_context");

    if (eglInitialize(gfxctx->display, &egl_version_major, &egl_version_minor)
     == EGL_FALSE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to initialize EGL: %s",
         get_egl_error_str(eglGetError()));

        goto egl_init_fail;
    }

    attrs_changed = process_attrs(&cfg_attrs, &ctx_attrs, egl_version_minor,
     egl_khr_create_context_supported);

    if (attrs_changed && !shared) {
        char gapi_str[GAPI_STR_SIZE_MAX];

        attrs_fill_gapi_str(gapi_str, &cfg_attrs, &ctx_attrs);

        module->logger.warning(module->logger.ctx,
         "gfxctx_egl: Unable to create context with required version. Possible "
         "reasons: required version of context or minor version of context may "
         "be not supported by current version of EGL or EGL_KHR_create_context "
         "extension is not present");
        module->logger.warning(module->logger.ctx,
         "gfxctx_egl: Current version of EGL: 1.%i", egl_version_minor);
        module->logger.warning(module->logger.ctx,
         "gfxctx_egl: Fallback context created: %s", gapi_str);
    }

    fill_cfg_attrs(egl_cfg_attrs, &cfg_attrs, egl_version_minor);
    fill_ctx_attrs(egl_ctx_attrs, &ctx_attrs, egl_version_minor,
     egl_khr_create_context_supported);

    if (shared)
        gfxctx->gapi = shared->gapi;
    else
        gfxctx->gapi = gapi_from_attrs(&cfg_attrs, &ctx_attrs);

    if (eglChooseConfig(gfxctx->display, egl_cfg_attrs, &gfxctx->cfg, 1,
     &configs_count) == EGL_FALSE || configs_count != 1) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to get matching frame buffer configuration: %s",
         get_egl_error_str(eglGetError()));

        goto choose_config_fail;
    }

    if (eglBindAPI(getegl_api_by_gapi((st_gapi_t)gfxctx->gapi)) == EGL_FALSE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to bind EGL API: %s",
         get_egl_error_str(eglGetError()));

        goto bind_api_fail;
    }

    gfxctx->surface = eglCreateWindowSurface(gfxctx->display,
     gfxctx->cfg,
     (EGLNativeWindowType)*(void *[]){module->window.get_handle(window)}, NULL);
    if (gfxctx->surface == EGL_NO_SURFACE) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to create EGL window surface: %s",
         get_egl_error_str(eglGetError()));

        goto create_surface_fail;
    }

    gfxctx->handle = eglCreateContext(gfxctx->display, gfxctx->cfg,
     shared ? shared->handle : EGL_NO_CONTEXT, egl_ctx_attrs);
    if (gfxctx->handle == EGL_NO_CONTEXT) {
        module->logger.error(module->logger.ctx,
         "gfxctx_egl: Unable to create EGL render context: %s",
         get_egl_error_str(eglGetError()));

        goto create_context_fail;
    }

    gfxctx->ctx = gfxctx_ctx;

    eglMakeCurrent(gfxctx->display, gfxctx->surface, gfxctx->surface,
     gfxctx->handle);
    if (eglSwapInterval(gfxctx->display, 1) == EGL_FALSE)
        module->logger.warning(module->logger.ctx,
         "gfxctx_egl: Unable to set swap interval: %s",
         get_egl_error_str(eglGetError()));

    return gfxctx;

create_context_fail:
    eglDestroySurface(gfxctx->display, gfxctx->surface);
bind_api_fail:
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
     get_minor_version_by_gapi(api), NULL);
}

static st_gfxctx_t *st_gfxctx_create_shared(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, st_gfxctx_t *other) {
    if (!monitor || !window || !other)
        return NULL;

    return st_gfxctx_create_impl(gfxctx_ctx, monitor, window,
     get_renderable_type_by_gapi((st_gapi_t)other->gapi),
     get_major_version_by_gapi((st_gapi_t)other->gapi),
     get_minor_version_by_gapi((st_gapi_t)other->gapi), other->handle);
}

static bool st_gfxctx_make_current(st_gfxctx_t *gfxctx) {
    return eglMakeCurrent(gfxctx->display, gfxctx->surface, gfxctx->surface,
     gfxctx->handle);
}

static bool st_gfxctx_swap_buffers(st_gfxctx_t *gfxctx) {
    return eglSwapBuffers(gfxctx->display, gfxctx->surface);
}

static st_modctx_t *st_gfxctx_get_ctx(st_gfxctx_t *gfxctx) {
    return gfxctx->ctx;
}

static st_gapi_t st_gfxctx_get_api(st_gfxctx_t *gfxctx) {
    return (st_gapi_t)gfxctx->gapi;
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
