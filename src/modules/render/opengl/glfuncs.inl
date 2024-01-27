#pragma once

#define SHADER_LOAD_FAIL_MSG                                                 \
 "Shaders will not be supported on this render ctx"
#define SHADER_DEL_LOAD_FAIL_MSG                                             \
 "Shaders will not be able to be deleted on this render ctx"
#define SHADER_LOG_LOAD_FAIL_MSG                                             \
 "Reporting shader compilation errors is not supported on this render ctx"
#define SHDPROG_DEL_LOAD_FAIL_MSG                                            \
 "Shader programs will not be able to be deleted on this render ctx"
#define SHDPROG_LOG_LOAD_FAIL_MSG                                            \
 "Reporting shader program linkage errors is not supported on this render ctx"
#define BUF_LOAD_FAIL_MSG                                                    \
 "Buffer objects will not be supported on this render ctx"
#define BUF_DEL_LOAD_FAIL_MSG                                                \
 "Buffer objects will not be able to be deleted on this render ctx"
#define BUF_SUBDATA_LOAD_FAIL_MSG                                            \
 "Buffer objects will not be able to update subdata on this render ctx. It " \
 "will update data instead"
#define VAO_LOAD_FAIL_MSG                                                    \
 "VAO will not be supported on this render ctx"
#define VAO_DEL_LOAD_FAIL_MSG                                                \
 "VAO will not be able to be deleted on this render ctx"

static bool glapi_least(st_gapi_t current_api, st_gapi_t req_api) {
    if (req_api < ST_GAPI_GL11 || req_api > ST_GAPI_GL46)
        return false;

    return current_api >= req_api;
}

static void *glfuncs_load_with_check(st_modctx_t *render_ctx, bool *prop_out,
 bool prop_check, const char *func_name, const char *err_msg) {
    st_render_opengl_t *module = render_ctx->data;
    void               *func = NULL;

    if (!prop_check)
        return NULL;

    func = module->glloader.get_proc_address(NULL, func_name);

    if (!func)
        module->logger.warning(module->logger.ctx,
         "render_opengl: Unable to load function \"%s\". %s", func_name,
         err_msg);

    *prop_out = !!func;

    module->logger.debug(module->logger.ctx, "render_opengl: Loaded function \"%s\"", func_name);

    return func;
}

static bool glfuncs_load_all(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;
    st_gapi_t           current_gapi = module->gfxctx.get_api(
     module->gfxctx.handle);
    errno_t             err = memset_s(&module->gl, sizeof(st_glfuncs_t), 0,
     sizeof(st_glfuncs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to set initial states of OpenGL function "
         "pointers: %s",
         err_msg_buf);

        return false;
    }

    err = memset_s(&module->glsupported, sizeof(st_glsupported_t), 0,
     sizeof(st_glsupported_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to set initial states of supported features: "
         "%s",
         err_msg_buf);

        return false;
    }

    /* Not tested on machines with OpenGL version <3.3 */
    assert(glapi_least(current_gapi, ST_GAPI_GL33));

    if (glapi_least(current_gapi, ST_GAPI_GL2)) {
        /* Shader - Main */
        module->gl.create_shader = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            true,
            "glCreateShader",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.shader_source = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glShaderSource",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.compile_shader = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glCompileShader",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.get_shader_iv = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glGetShaderiv",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.create_program = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glCreateProgram",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.attach_shader = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glAttachShader",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.link_program = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glLinkProgram",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.get_program_iv = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glGetProgramiv",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.use_program = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glUseProgram",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.get_attrib_location = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glGetAttribLocation",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.vertex_attrib_pointer = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glVertexAttribPointer",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.enable_vertex_attrib_array = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glEnableVertexAttribArray",
            SHADER_LOAD_FAIL_MSG
        );
        module->gl.disable_vertex_attrib_array = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_main,
            module->glsupported.shader_main,
            "glDisableVertexAttribArray",
            SHADER_LOAD_FAIL_MSG
        );
        /* Shader - Delete */
        module->gl.delete_shader = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_del,
            module->glsupported.shader_main,
            "glDeleteShader",
            SHADER_DEL_LOAD_FAIL_MSG
        );
        /* Shader - Log */
        module->gl.get_shader_info_log = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shader_log,
            module->glsupported.shader_main,
            "glGetShaderInfoLog",
            SHADER_LOG_LOAD_FAIL_MSG
        );
        /* Shader program - Delete */
        module->gl.delete_program = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shdprog_del,
            module->glsupported.shader_main,
            "glDeleteProgram",
            SHDPROG_DEL_LOAD_FAIL_MSG
        );
        /* Shader program - Log */
        module->gl.get_program_info_log = glfuncs_load_with_check(render_ctx,
            &module->glsupported.shdprog_log,
            module->glsupported.shader_main,
            "glGetProgramInfoLog",
            SHDPROG_LOG_LOAD_FAIL_MSG
        );
    } else if (glapi_least(current_gapi, ST_GAPI_GL1)) {
        /*
         * TODO someday (or not)
         *
         * https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shader_objects.txt
         *  CreateShaderObjectARB
         *  ShaderSourceARB
         *  CompileShaderARB
         *  GetObjectParameterivARB
         *  CreateProgramObjectARB
         *  AttachObjectARB
         *  LinkProgramARB
         *  UseProgramObjectARB
         *  GetInfoLogARB
         *  DeleteObjectARB
         * https://registry.khronos.org/OpenGL/extensions/ARB/ARB_vertex_shader.txt
         *  GetAttribLocationARB
         *  VertexAttribPointerARB
         *  EnableVertexAttribArrayARB
         *  DisableVertexAttribArrayARB
         *
         * Also required:
         *  https://registry.khronos.org/OpenGL/extensions/ARB/ARB_fragment_shader.txt
         *  https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shading_language_100.txt
         *
         * Non-ARB extensions are not suitable, because we need GLSL
         */
    }

    if (glapi_least(current_gapi, ST_GAPI_GL15)) {
        /* Buffer - Main */
        module->gl.gen_buffers = glfuncs_load_with_check(render_ctx,
            &module->glsupported.buf_main,
            true,
            "glGenBuffers",
            BUF_LOAD_FAIL_MSG
        );
        module->gl.bind_buffer = glfuncs_load_with_check(render_ctx,
            &module->glsupported.buf_main,
            module->glsupported.buf_main,
            "glBindBuffer",
            BUF_LOAD_FAIL_MSG
        );
        module->gl.buffer_data = glfuncs_load_with_check(render_ctx,
            &module->glsupported.buf_main,
            module->glsupported.buf_main,
            "glBufferData",
            BUF_LOAD_FAIL_MSG
        );
        /* Buffer - Delete */
        module->gl.delete_buffers = glfuncs_load_with_check(render_ctx,
            &module->glsupported.buf_del,
            module->glsupported.buf_main,
            "glDeleteBuffers",
            BUF_DEL_LOAD_FAIL_MSG
        );
        /* Buffer - Subdata */
        module->gl.buffer_sub_data = glfuncs_load_with_check(render_ctx,
            &module->glsupported.buf_subdata,
            module->glsupported.buf_main,
            "glBufferSubData",
            BUF_SUBDATA_LOAD_FAIL_MSG
        );
    } else if (glapi_least(current_gapi, ST_GAPI_GL14)) {
        /*
         * TODO someday (or not)
         *
         * https://registry.khronos.org/OpenGL/extensions/ARB/ARB_vertex_buffer_object.txt
         *  GenBuffersARB
         *  BindBufferARB
         *  BufferDataARB
         *  DeleteBuffersARB
         *  BufferSubDataARB
         */
    } else if (glapi_least(current_gapi, ST_GAPI_GL11)) {
        /*
         * TODO someday (or not)
         *
         * https://registry.khronos.org/OpenGL/extensions/ATI/ATI_vertex_array_object.txt
         *  NewObjectBufferATI
         *  UpdateObjectBufferATI
         *  FreeObjectBufferATI
         *  ArrayObjectATI
         */
    } else if (glapi_least(current_gapi, ST_GAPI_GL1)) {
        /*
         * TODO someday (or not)
         *
         * https://registry.khronos.org/OpenGL/extensions/NV/NV_vertex_array_range.txt
         */
    }

    if (glapi_least(current_gapi, ST_GAPI_GL3)) {
        /* VAO - Main */
        module->gl.gen_vertex_arrays = glfuncs_load_with_check(render_ctx,
            &module->glsupported.vao_main,
            module->glsupported.shader_main || module->glsupported.buf_main,
            "glGenVertexArrays",
            VAO_LOAD_FAIL_MSG
        );
        module->gl.bind_vertex_array = glfuncs_load_with_check(render_ctx,
            &module->glsupported.vao_main,
            module->glsupported.vao_main,
            "glBindVertexArray",
            VAO_LOAD_FAIL_MSG
        );
        /* VAO - Delete */
        module->gl.delete_vertex_arrays = glfuncs_load_with_check(render_ctx,
            &module->glsupported.vao_delete,
            module->glsupported.vao_main,
            "glDeleteVertexArrays",
            VAO_DEL_LOAD_FAIL_MSG
        );
    } else if (glapi_least(current_gapi, ST_GAPI_GL21)) {
        /*
         * TODO someday (or not)
         *
         * https://registry.khronos.org/OpenGL/extensions/ARB/ARB_vertex_array_object.txt
         *  GenVertexArrays
         *  BindVertexArray
         *  DeleteVertexArrays
         */
    } else if (glapi_least(current_gapi, ST_GAPI_GL1)) {
        /*
         * TODO someday (or not)
         *
         * https://registry.khronos.org/OpenGL/extensions/APPLE/APPLE_vertex_array_object.txt
         *  GenVertexArraysAPPLE
         *  BindVertexArrayAPPLE
         *  DeleteVertexArraysAPPLE
         */
    }

    if (glapi_least(current_gapi, ST_GAPI_GL3)
     && (!module->glsupported.shader_main || !module->glsupported.buf_main)) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Shaders and Buffer objects required for OpenGL >=3.0");

        return false;
    }
    if (glapi_least(current_gapi, ST_GAPI_GL32)
     && !module->glsupported.vao_main) {
        module->logger.error(module->logger.ctx,
         "render_opengl: VAO required for OpenGL >=3.2");

        return false;
    }

    return true;
}
