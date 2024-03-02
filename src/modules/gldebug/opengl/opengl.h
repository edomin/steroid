#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/gldebug.h"

st_gldebug_funcs_t st_gldebug_opengl_funcs = {
    .gldebug_init                = st_gldebug_init,
    .gldebug_quit                = st_gldebug_quit,
    .gldebug_label_buffer        = st_gldebug_label_buffer,
    .gldebug_label_shader        = st_gldebug_label_shader,
    .gldebug_label_shdprog       = st_gldebug_label_shdprog,
    .gldebug_label_vao           = st_gldebug_label_vao,
    .gldebug_label_pipeline      = st_gldebug_label_pipeline,
    .gldebug_label_texture       = st_gldebug_label_texture,
    .gldebug_label_framebuffer   = st_gldebug_label_framebuffer,
    .gldebug_unlabel_buffer      = st_gldebug_unlabel_buffer,
    .gldebug_unlabel_shader      = st_gldebug_unlabel_shader,
    .gldebug_unlabel_shdprog     = st_gldebug_unlabel_shdprog,
    .gldebug_unlabel_vao         = st_gldebug_unlabel_vao,
    .gldebug_unlabel_pipeline    = st_gldebug_unlabel_pipeline,
    .gldebug_unlabel_texture     = st_gldebug_unlabel_texture,
    .gldebug_unlabel_framebuffer = st_gldebug_unlabel_framebuffer,
    .gldebug_get_error_msg       = st_gldebug_get_error_msg,
};

st_modfuncentry_t st_module_gldebug_opengl_funcs[] = {
    {"init",                st_gldebug_init},
    {"quit",                st_gldebug_quit},
    {"label_buffer",        st_gldebug_label_buffer},
    {"label_shader",        st_gldebug_label_shader},
    {"label_shdprog",       st_gldebug_label_shdprog},
    {"label_vao",           st_gldebug_label_vao},
    {"label_pipeline",      st_gldebug_label_pipeline},
    {"label_texture",       st_gldebug_label_texture},
    {"label_framebuffer",   st_gldebug_label_framebuffer},
    {"unlabel_buffer",      st_gldebug_unlabel_buffer},
    {"unlabel_shader",      st_gldebug_unlabel_shader},
    {"unlabel_shdprog",     st_gldebug_unlabel_shdprog},
    {"unlabel_vao",         st_gldebug_unlabel_vao},
    {"unlabel_pipeline",    st_gldebug_unlabel_pipeline},
    {"unlabel_texture",     st_gldebug_unlabel_texture},
    {"unlabel_framebuffer", st_gldebug_unlabel_framebuffer},
    {"get_error_msg",       st_gldebug_get_error_msg},
    {NULL, NULL},
};
