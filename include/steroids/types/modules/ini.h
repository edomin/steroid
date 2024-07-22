#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/object.h"

#ifndef ST_INI_T_DEFINED
    typedef struct st_ini_s st_ini_t;
#endif

typedef st_modctx_t *(*st_ini_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_ini_quit_t)(st_modctx_t *ini_ctx);

typedef st_ini_t *(*st_ini_create_t)(st_modctx_t *ini_ctx);
typedef st_ini_t *(*st_ini_load_t)(st_modctx_t *ini_ctx, const char *filename);
typedef st_ini_t *(*st_ini_memload_t)(st_modctx_t *ini_ctx, const void *ptr,
 size_t size);
typedef void (*st_ini_destroy_t)(st_ini_t *ini);
typedef bool (*st_ini_section_exists_t)(const st_ini_t *ini,
 const char *section);
typedef bool (*st_ini_key_exists_t)(const st_ini_t *ini, const char *section,
 const char *key);
typedef const char *(*st_ini_get_str_t)(const st_ini_t *ini,
 const char *section, const char *key);
typedef bool (*st_ini_fill_str_t)(const st_ini_t *ini, char *dst,
 size_t dstsize, const char *section, const char *key);
typedef bool (*st_ini_delete_section_t)(st_ini_t *ini, const char *section);
typedef bool (*st_ini_delete_key_t)(st_ini_t *ini, const char *section,
 const char *key);
typedef bool (*st_ini_clear_section_t)(st_ini_t *ini, const char *section);
typedef bool (*st_ini_add_section_t)(st_ini_t *ini, const char *section);
typedef bool (*st_ini_add_key_t)(st_ini_t *ini, const char *section,
 const char *key, const char *value);
typedef bool (*st_ini_export_t)(const st_ini_t *ini, char *buffer,
 size_t bufsize);
typedef bool (*st_ini_save_t)(const st_ini_t *ini, const char *filename);

typedef struct {
    st_ini_init_t    ini_init;
    st_ini_quit_t    ini_quit;
    st_ini_load_t    ini_load;
    st_ini_memload_t ini_memload;
    st_ini_create_t  ini_create;
} st_inictx_funcs_t;

typedef struct {
    st_ini_destroy_t        destroy;
    st_ini_section_exists_t section_exists;
    st_ini_key_exists_t     key_exists;
    st_ini_get_str_t        get_str;
    st_ini_fill_str_t       fill_str;
    st_ini_delete_section_t delete_section;
    st_ini_delete_key_t     delete_key;
    st_ini_clear_section_t  clear_section;
    st_ini_add_section_t    add_section;
    st_ini_add_key_t        add_key;
    st_ini_export_t         to_buffer;
    st_ini_save_t           save;
} st_ini_funcs_t;

#define ST_INI_CALL(object, func, ...) \
    ((st_ini_funcs_t *)((const st_object_t *)object)->funcs)->func(object, ## __VA_ARGS__)
