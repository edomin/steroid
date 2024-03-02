#include "opengl.h"

#include <stdio.h>

#include <GL/gl.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(gldebug_opengl)
ST_MODULE_DEF_INIT_FUNC(gldebug_opengl)

#ifdef _WIN32
    #define MINIMAL_OPENGL "1.1"
#elif __linux__
    #define MINIMAL_OPENGL "1.2"
#else
    #error Unknown target OS
#endif

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_gldebug_opengl_init(modsmgr, modsmgr_funcs);
}
#endif

static bool extension_supported(const char *ext) {
    const char *extensions = glGetString(GL_EXTENSIONS);

    return extensions && strstr(extensions, ext);
}

static bool glapi_least(st_gapi_t current_api, st_gapi_t req_api) {
    return req_api >= ST_GAPI_GL1
        && req_api <= ST_GAPI_GL46
        && current_api >= req_api;
}

static void *glfuncs_load_with_check(st_gldebug_opengl_logger_t *logger,
 st_modctx_t *glloader_ctx, st_glloader_get_proc_address_t get_proc_address,
 bool *prop_out, bool prop_check, const char *func_name) {
    void *func = NULL;

    if (!prop_check)
        return NULL;

    func = get_proc_address(glloader_ctx, func_name);

    *prop_out = !!func;

    return func;
}

void impl_set_callback_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx) {}

void impl_init_control_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx) {}

void impl_remove_callback_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx) {}

void impl_label_buffer_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id,
 __attribute__((unused)) const char *label) {}

void impl_label_shader_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused))  unsigned id,
 __attribute__((unused)) const char *label) {}

void impl_label_shdprog_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id,
 __attribute__((unused)) const char *label) {}

void impl_label_vao_stub(__attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id,
 __attribute__((unused)) const char *label) {}

void impl_label_pipeline_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id,
 __attribute__((unused)) const char *label) {}

void impl_label_texture_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id,
 __attribute__((unused)) const char *label) {}

void impl_label_framebuffer_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id,
 __attribute__((unused)) const char *label) {}

void impl_unlabel_buffer_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id) {}

void impl_unlabel_shader_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused))  unsigned id) {}

void impl_unlabel_shdprog_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id) {}

void impl_unlabel_vao_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id) {}

void impl_unlabel_pipeline_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id) {}

void impl_unlabel_texture_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id) {}

void impl_unlabel_framebuffer_stub(
 __attribute__((unused)) const st_modctx_t *gldebug_ctx,
 __attribute__((unused)) unsigned id) {}

static st_logger_generic_msg_t severity_to_logger_func(
 const st_gldebug_opengl_t *module, GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            return module->logger.error;
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW:
            return module->logger.warning;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        default:
            return module->logger.info;
    };

    return module->logger.info;
}

static void callback(__attribute__((unused)) GLenum source,
 __attribute__((unused)) GLenum type, __attribute__((unused)) GLuint id,
 GLenum severity, __attribute__((unused)) GLsizei length, const GLchar* message,
 const void* userParam) {
    const st_gldebug_opengl_t *module = userParam;

    severity_to_logger_func(module, severity)(module->logger.ctx,
     "gldebug_opengl: %s", message);
}

static void callback_amd(__attribute__((unused)) GLuint id,
 __attribute__((unused)) GLenum category, GLenum severity,
 __attribute__((unused)) GLsizei length, const GLchar* message,
 GLvoid* userParam) {
    const st_gldebug_opengl_t *module = userParam;

    severity_to_logger_func(module, severity)(module->logger.ctx,
     "gldebug_opengl: %s", message);
}

static void impl_set_callback_core(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_callback(callback, module);
}

static void impl_init_control_core(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_control(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
     0, NULL, GL_TRUE);
}

static void impl_remove_callback_core(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_callback(NULL, NULL);
}

static void impl_label_buffer_core(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_BUFFER, id, -1, label);
}

static void impl_label_shader_core(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_SHADER, id, -1, label);
}

static void impl_label_shdprog_core(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_PROGRAM, id, -1, label);
}

static void impl_label_vao_core(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_VERTEX_ARRAY, id, -1, label);
}

static void impl_label_pipeline_core(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_PROGRAM_PIPELINE, id, -1, label);
}

static void impl_label_texture_core(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_TEXTURE, id, -1, label);
}

static void impl_label_framebuffer_core(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_FRAMEBUFFER, id, -1, label);
}

static void impl_unlabel_buffer_core(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_BUFFER, id, -1, NULL);
}

static void impl_unlabel_shader_core(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_SHADER, id, -1, NULL);
}

static void impl_unlabel_shdprog_core(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_PROGRAM, id, -1, NULL);
}

static void impl_unlabel_vao_core(const st_modctx_t *gldebug_ctx, unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_VERTEX_ARRAY, id, -1, NULL);
}

static void impl_unlabel_pipeline_core(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_PROGRAM_PIPELINE, id, -1, NULL);
}

static void impl_unlabel_texture_core(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_TEXTURE, id, -1, NULL);
}

static void impl_unlabel_framebuffer_core(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.object_label(GL_FRAMEBUFFER, id, -1, NULL);
}

static void impl_set_callback_arb(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_callback_arb(callback, module);
}

static void impl_init_control_arb(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_control_arb(GL_DONT_CARE, GL_DONT_CARE,
     GL_DONT_CARE, 0, NULL, GL_TRUE);
}

static void impl_remove_callback_arb(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_callback_arb(NULL, NULL);
}

static void impl_set_callback_amd(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_callback_amd(callback_amd, module);
}

static void impl_init_control_amd(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_enable_amd(0, 0, 0, NULL, GL_TRUE);
}

static void impl_remove_callback_amd(const st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->gl.debug_message_callback_amd(NULL, NULL);
}

static void load_gl_funcs(st_modctx_t *gldebug_ctx, st_modctx_t *glloader_ctx,
 st_glloader_get_proc_address_t get_proc_address) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    if (!get_proc_address) {
        module->agn.set_callback        = impl_set_callback_stub;
        module->agn.init_control        = impl_init_control_stub;
        module->agn.remove_callback     = impl_remove_callback_stub;
        module->agn.label_buffer        = impl_label_buffer_stub;
        module->agn.label_shader        = impl_label_shader_stub;
        module->agn.label_shdprog       = impl_label_shdprog_stub;
        module->agn.label_vao           = impl_label_vao_stub;
        module->agn.label_pipeline      = impl_label_pipeline_stub;
        module->agn.label_texture       = impl_label_texture_stub;
        module->agn.label_framebuffer   = impl_label_framebuffer_stub;
        module->agn.unlabel_buffer      = impl_unlabel_buffer_stub;
        module->agn.unlabel_shader      = impl_unlabel_shader_stub;
        module->agn.unlabel_shdprog     = impl_unlabel_shdprog_stub;
        module->agn.unlabel_vao         = impl_unlabel_vao_stub;
        module->agn.unlabel_pipeline    = impl_unlabel_pipeline_stub;
        module->agn.unlabel_texture     = impl_unlabel_texture_stub;
        module->agn.unlabel_framebuffer = impl_unlabel_framebuffer_stub;

        return;
    }

    if (glapi_least(module->gfxctx.api, ST_GAPI_GL43)
     || (glapi_least(module->gfxctx.api, ST_GAPI_GL11)
     && extension_supported("GL_KHR_debug"))) {
        /* Callback - Main */
        module->gl.debug_message_callback = glfuncs_load_with_check(
         &module->logger, glloader_ctx, get_proc_address,
            &module->glsupported.cbk_main,
            true,
            "glDebugMessageCallback"
        );
        /* Callback - Control */
        module->gl.debug_message_control = glfuncs_load_with_check(
         &module->logger, glloader_ctx, get_proc_address,
            &module->glsupported.cbk_ctrl,
            module->glsupported.cbk_main,
            "glDebugMessageControl"
        );
        /* Callback - Label */
        module->gl.object_label = glfuncs_load_with_check(&module->logger,
         glloader_ctx, get_proc_address,
            &module->glsupported.cbk_label,
            module->glsupported.cbk_main,
            "glObjectLabel"
        );
        module->gl.object_ptr_label = glfuncs_load_with_check(&module->logger,
         glloader_ctx, get_proc_address,
            &module->glsupported.cbk_label,
            module->glsupported.cbk_label,
            "glObjectPtrLabel"
        );
        module->gl.get_object_label = glfuncs_load_with_check(&module->logger,
         glloader_ctx, get_proc_address,
            &module->glsupported.cbk_label,
            module->glsupported.cbk_label,
            "glGetObjectLabel"
        );
        module->gl.get_object_ptr_label = glfuncs_load_with_check(
         &module->logger, glloader_ctx, get_proc_address,
            &module->glsupported.cbk_label,
            module->glsupported.cbk_label,
            "glGetObjectPtrLabel"
        );
        if (module->glsupported.cbk_main)
            module->glsupported.cbk_ext = EXT_CORE;
    } else if (glapi_least(module->gfxctx.api, ST_GAPI_GL11)
     && extension_supported("GL_ARB_debug_output")) {
        /* Callback - Main */
        module->gl.debug_message_callback_arb = glfuncs_load_with_check(
         &module->logger, glloader_ctx, get_proc_address,
            &module->glsupported.cbk_main,
            true,
            "glDebugMessageCallbackARB"
        );
        /* Callback - Control */
        module->gl.debug_message_control_arb = glfuncs_load_with_check(
         &module->logger, glloader_ctx, get_proc_address,
            &module->glsupported.cbk_ctrl,
            module->glsupported.cbk_main,
            "glDebugMessageControlARB"
        );
        if (module->glsupported.cbk_main)
            module->glsupported.cbk_ext = EXT_ARB;
    } else if (glapi_least(module->gfxctx.api, ST_GAPI_GL11)
     && extension_supported("GL_AMD_debug_output")) {
        /* Callback - Main */
        module->gl.debug_message_callback_amd = glfuncs_load_with_check(
         &module->logger, glloader_ctx, get_proc_address,
            &module->glsupported.cbk_main,
            true,
            "glDebugMessageCallbackAMD"
        );
        /* Callback - Control */
        module->gl.debug_message_enable_amd = glfuncs_load_with_check(
         &module->logger, glloader_ctx, get_proc_address,
            &module->glsupported.cbk_ctrl,
            module->glsupported.cbk_main,
            "glDebugMessageEnableAMD"
        );
        if (module->glsupported.cbk_main)
            module->glsupported.cbk_ext = EXT_AMD;
    }

    if (module->glsupported.cbk_ext == EXT_CORE) {
        if (module->glsupported.cbk_main) {
            module->agn.set_callback = impl_set_callback_core;
            module->agn.remove_callback = impl_remove_callback_core;
        } else {
            module->agn.set_callback = impl_set_callback_stub;
            module->agn.remove_callback = impl_remove_callback_stub;
        }
        module->agn.init_control = module->glsupported.cbk_ctrl
            ? impl_init_control_core
            : impl_init_control_stub;

        if (module->glsupported.cbk_label) {
            module->agn.label_buffer        = impl_label_buffer_core;
            module->agn.label_shader        = impl_label_shader_core;
            module->agn.label_shdprog       = impl_label_shdprog_core;
            module->agn.label_vao           = impl_label_vao_core;
            module->agn.label_pipeline      = impl_label_pipeline_core;
            module->agn.label_texture       = impl_label_texture_core;
            module->agn.label_framebuffer   = impl_label_framebuffer_core;
            module->agn.unlabel_buffer      = impl_unlabel_buffer_core;
            module->agn.unlabel_shader      = impl_unlabel_shader_core;
            module->agn.unlabel_shdprog     = impl_unlabel_shdprog_core;
            module->agn.unlabel_vao         = impl_unlabel_vao_core;
            module->agn.unlabel_pipeline    = impl_unlabel_pipeline_core;
            module->agn.unlabel_texture     = impl_unlabel_texture_core;
            module->agn.unlabel_framebuffer = impl_unlabel_framebuffer_core;
        } else {
            module->agn.label_buffer        = impl_label_buffer_stub;
            module->agn.label_shader        = impl_label_shader_stub;
            module->agn.label_shdprog       = impl_label_shdprog_stub;
            module->agn.label_vao           = impl_label_vao_stub;
            module->agn.label_pipeline      = impl_label_pipeline_stub;
            module->agn.label_texture       = impl_label_texture_stub;
            module->agn.label_framebuffer   = impl_label_framebuffer_stub;
            module->agn.unlabel_buffer      = impl_unlabel_buffer_stub;
            module->agn.unlabel_shader      = impl_unlabel_shader_stub;
            module->agn.unlabel_shdprog     = impl_unlabel_shdprog_stub;
            module->agn.unlabel_vao         = impl_unlabel_vao_stub;
            module->agn.unlabel_pipeline    = impl_unlabel_pipeline_stub;
            module->agn.unlabel_texture     = impl_unlabel_texture_stub;
            module->agn.unlabel_framebuffer = impl_unlabel_framebuffer_stub;
        }
    } else if (module->glsupported.cbk_ext == EXT_ARB) {
        if (module->glsupported.cbk_main) {
            module->agn.set_callback = impl_set_callback_arb;
            module->agn.remove_callback = impl_remove_callback_arb;
        } else {
            module->agn.set_callback = impl_set_callback_stub;
            module->agn.remove_callback = impl_remove_callback_stub;
        }
        module->agn.init_control = module->glsupported.cbk_ctrl
            ? impl_init_control_arb
            : impl_init_control_stub;

        module->agn.label_buffer        = impl_label_buffer_stub;
        module->agn.label_shader        = impl_label_shader_stub;
        module->agn.label_shdprog       = impl_label_shdprog_stub;
        module->agn.label_vao           = impl_label_vao_stub;
        module->agn.label_pipeline      = impl_label_pipeline_stub;
        module->agn.label_texture       = impl_label_texture_stub;
        module->agn.label_framebuffer   = impl_label_framebuffer_stub;
        module->agn.unlabel_buffer      = impl_unlabel_buffer_stub;
        module->agn.unlabel_shader      = impl_unlabel_shader_stub;
        module->agn.unlabel_shdprog     = impl_unlabel_shdprog_stub;
        module->agn.unlabel_vao         = impl_unlabel_vao_stub;
        module->agn.unlabel_pipeline    = impl_unlabel_pipeline_stub;
        module->agn.unlabel_texture     = impl_unlabel_texture_stub;
        module->agn.unlabel_framebuffer = impl_unlabel_framebuffer_stub;
    } else if (module->glsupported.cbk_ext == EXT_AMD) {
        if (module->glsupported.cbk_main) {
            module->agn.set_callback = impl_set_callback_amd;
            module->agn.remove_callback = impl_remove_callback_amd;
        } else {
            module->agn.set_callback = impl_set_callback_stub;
            module->agn.remove_callback = impl_remove_callback_stub;
        }
        module->agn.init_control = module->glsupported.cbk_ctrl
            ? impl_init_control_amd
            : impl_init_control_stub;

        module->agn.label_buffer        = impl_label_buffer_stub;
        module->agn.label_shader        = impl_label_shader_stub;
        module->agn.label_shdprog       = impl_label_shdprog_stub;
        module->agn.label_vao           = impl_label_vao_stub;
        module->agn.label_pipeline      = impl_label_pipeline_stub;
        module->agn.label_texture       = impl_label_texture_stub;
        module->agn.label_framebuffer   = impl_label_framebuffer_stub;
        module->agn.unlabel_buffer      = impl_unlabel_buffer_stub;
        module->agn.unlabel_shader      = impl_unlabel_shader_stub;
        module->agn.unlabel_shdprog     = impl_unlabel_shdprog_stub;
        module->agn.unlabel_vao         = impl_unlabel_vao_stub;
        module->agn.unlabel_pipeline    = impl_unlabel_pipeline_stub;
        module->agn.unlabel_texture     = impl_unlabel_texture_stub;
        module->agn.unlabel_framebuffer = impl_unlabel_framebuffer_stub;
        module->agn.remove_callback     = impl_remove_callback_stub;
    } else {
        module->agn.set_callback        = impl_set_callback_stub;
        module->agn.init_control        = impl_init_control_stub;
        module->agn.label_buffer        = impl_label_buffer_stub;
        module->agn.label_shader        = impl_label_shader_stub;
        module->agn.label_shdprog       = impl_label_shdprog_stub;
        module->agn.label_vao           = impl_label_vao_stub;
        module->agn.label_pipeline      = impl_label_pipeline_stub;
        module->agn.label_texture       = impl_label_texture_stub;
        module->agn.label_framebuffer   = impl_label_framebuffer_stub;
        module->agn.unlabel_buffer      = impl_unlabel_buffer_stub;
        module->agn.unlabel_shader      = impl_unlabel_shader_stub;
        module->agn.unlabel_shdprog     = impl_unlabel_shdprog_stub;
        module->agn.unlabel_vao         = impl_unlabel_vao_stub;
        module->agn.unlabel_pipeline    = impl_unlabel_pipeline_stub;
        module->agn.unlabel_texture     = impl_unlabel_texture_stub;
        module->agn.unlabel_framebuffer = impl_unlabel_framebuffer_stub;
        module->agn.remove_callback     = impl_remove_callback_stub;
    }
}

static bool st_gldebug_import_functions(st_modctx_t *gldebug_ctx,
 st_modctx_t *logger_ctx, st_gfxctx_t *gfxctx) {
    st_gldebug_opengl_t           *module = gldebug_ctx->data;
    st_gfxctx_get_ctx_t            st_gfxctx_get_ctx;
    st_gfxctx_get_api_t            st_gfxctx_get_api;
    st_glloader_init_t             st_glloader_init;
    st_glloader_quit_t             st_glloader_quit;
    st_glloader_get_proc_address_t st_glloader_get_proc_address = NULL;
    st_modctx_t                   *gfxctx_ctx;
    st_modctx_t                   *glloader_ctx;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "gldebug_opengl: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    st_gfxctx_get_ctx = global_modsmgr_funcs.get_function(global_modsmgr,
     "gfxctx", NULL, "get_ctx");
    if (!st_gfxctx_get_ctx) {
        module->logger.error(module->logger.ctx,
         "gldebug_opengl: Unable to load function \"get_ctx\" from module "
         "\"gfxctx\"\n");

        return false;
    }

    gfxctx_ctx = st_gfxctx_get_ctx(gfxctx);
    if (gfxctx_ctx)
        st_gfxctx_get_api = global_modsmgr_funcs.get_function_from_ctx(
         global_modsmgr, gfxctx_ctx, "get_api");

    if (!st_gfxctx_get_api) {
        module->logger.error(module->logger.ctx,
         "gldebug_opengl: Unable to load function \"get_api\" from module "
         "\"gfxctx\"\n");

        return false;
    }

    module->gfxctx.api = st_gfxctx_get_api(gfxctx);

    ST_LOAD_FUNCTION_FROM_CTX("gldebug_opengl", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("gldebug_opengl", logger, info);
    ST_LOAD_FUNCTION_FROM_CTX("gldebug_opengl", logger, warning);

    ST_LOAD_FUNCTION_FROM_CTX("gldebug_opengl", gfxctx, make_current);
    ST_LOAD_FUNCTION_FROM_CTX("gldebug_opengl", gfxctx, set_userdata);
    ST_LOAD_FUNCTION_FROM_CTX("gldebug_opengl", gfxctx, get_userdata);

    st_glloader_init = global_modsmgr_funcs.get_function(global_modsmgr,
     "glloader", gfxctx_ctx->name, "init");
    if (!st_glloader_init) {
        module->logger.warning(module->logger.ctx,
         "gldebug_opengl: Unable to load function \"init\" from module "
         "\"glloader\". This is why unable to use OpenGL functions above "
         "OpenGL %s and extensions\n", MINIMAL_OPENGL);
    }

    if (st_glloader_init)
        glloader_ctx = st_glloader_init(logger_ctx, gfxctx);
    if (glloader_ctx) {
        st_glloader_quit = global_modsmgr_funcs.get_function_from_ctx(
         global_modsmgr, glloader_ctx, "quit");
        if (!st_glloader_quit) {
            module->logger.warning(module->logger.ctx,
             "gldebug_opengl: Unable to load function \"quit\" from module "
             "\"glloader\"\n");
        }

        st_glloader_get_proc_address =
         global_modsmgr_funcs.get_function_from_ctx(global_modsmgr,
         glloader_ctx, "get_proc_address");
        if (st_glloader_get_proc_address) {

        } else {
            module->logger.error(module->logger.ctx,
             "gldebug_opengl: Unable to load function \"get_proc_address\" "
             "from module \"gllogger\"\n");
        }

        if (st_glloader_quit)
            st_glloader_quit(glloader_ctx);
    }

    load_gl_funcs(gldebug_ctx, glloader_ctx, st_glloader_get_proc_address);

    return true;
}

static st_modctx_t *st_gldebug_init(st_modctx_t *logger_ctx,
 st_gfxctx_t *gfxctx) {
    st_gldebug_opengl_t *module;
    st_modctx_t         *gldebug_ctx = global_modsmgr_funcs.init_module_ctx(
     global_modsmgr, &st_module_gldebug_opengl_data,
     sizeof(st_gldebug_opengl_t));

    if (!gldebug_ctx)
        return NULL;

    gldebug_ctx->funcs = &st_gldebug_opengl_funcs;

    module = gldebug_ctx->data;
    module->logger.ctx = logger_ctx;
    module->gfxctx.handle = gfxctx;
    module->gl = (st_gldebug_glfuncs_t){0};
    module->glsupported = (st_gldebug_glsupported_t){0};

    if (!st_gldebug_import_functions(gldebug_ctx, logger_ctx, gfxctx))
        goto func_import_fail;




    if (module->glsupported.cbk_main) {
        uintptr_t gldebug_ref_counter;

        if (module->glsupported.cbk_ext == EXT_CORE) {
            GLint gfxctx_flags;

            glGetIntegerv(GL_CONTEXT_FLAGS, &gfxctx_flags);
            if (gfxctx_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            }
        } else if (module->glsupported.cbk_ext == EXT_ARB) {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        }

        module->agn.set_callback(gldebug_ctx);

        if (!module->gfxctx.get_userdata(gfxctx, &gldebug_ref_counter,
         "gldebug_ref_counter"))
            gldebug_ref_counter = 0;

        module->gfxctx.set_userdata(gfxctx, "gldebug_ref_counter",
         ++gldebug_ref_counter);
    }

    if (module->glsupported.cbk_main) {

    }

    module->logger.info(module->logger.ctx,
     "gldebug_opengl: OpenGL debug initialized");

    return gldebug_ctx;

func_import_fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, gldebug_ctx);

    return NULL;
}

static void st_gldebug_quit(st_modctx_t *gldebug_ctx) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    if (module->glsupported.cbk_main) {
        uintptr_t gldebug_ref_counter;

        if (module->gfxctx.get_userdata(module->gfxctx.handle,
         &gldebug_ref_counter, "gldebug_ref_counter")) {
            module->gfxctx.set_userdata(module->gfxctx.handle,
             "gldebug_ref_counter", --gldebug_ref_counter);

            if (gldebug_ref_counter == 0)
                module->agn.remove_callback(gldebug_ctx);
        }
    }

    module->logger.info(module->logger.ctx,
     "texture_opengl: Texture mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, gldebug_ctx);
}

static void st_gldebug_label_buffer(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.label_buffer(gldebug_ctx, id, label);
}

static void st_gldebug_label_shader(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.label_shader(gldebug_ctx, id, label);
}

static void st_gldebug_label_shdprog(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.label_shdprog(gldebug_ctx, id, label);
}

static void st_gldebug_label_vao(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.label_vao(gldebug_ctx, id, label);
}

static void st_gldebug_label_pipeline(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.label_pipeline(gldebug_ctx, id, label);
}

static void st_gldebug_label_texture(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.label_texture(gldebug_ctx, id, label);
}

static void st_gldebug_label_framebuffer(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.label_framebuffer(gldebug_ctx, id, label);
}

static void st_gldebug_unlabel_buffer(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.unlabel_buffer(gldebug_ctx, id);
}

static void st_gldebug_unlabel_shader(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.unlabel_shader(gldebug_ctx, id);
}

static void st_gldebug_unlabel_shdprog(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.unlabel_shdprog(gldebug_ctx, id);
}

static void st_gldebug_unlabel_vao(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.unlabel_vao(gldebug_ctx, id);
}

static void st_gldebug_unlabel_pipeline(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.unlabel_pipeline(gldebug_ctx, id);
}

static void st_gldebug_unlabel_texture(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.unlabel_texture(gldebug_ctx, id);
}

static void st_gldebug_unlabel_framebuffer(const st_modctx_t *gldebug_ctx,
 unsigned id) {
    st_gldebug_opengl_t *module = gldebug_ctx->data;

    module->agn.unlabel_framebuffer(gldebug_ctx, id);
}

static const char *st_gldebug_get_error_msg(const st_modctx_t *gldebug_ctx) {
    switch (glGetError()) {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "Invalid framebuffer operation";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        case GL_STACK_UNDERFLOW:
            return "Stack underflow";
        case GL_STACK_OVERFLOW:
            return "Stack overflow";
        default:
            break;
    }

    return "Unknown error";
}
