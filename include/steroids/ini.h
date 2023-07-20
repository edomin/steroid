#ifndef ST_STEROIDS_INI_H
#define ST_STEROIDS_INI_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/ini.h"

static st_modctx_t *st_ini_init(st_modctx_t *logger_ctx);
static void st_ini_quit(st_modctx_t *ini_ctx);

static st_ini_t *st_ini_create(st_modctx_t *ini_ctx);
static st_ini_t *st_ini_load(st_modctx_t *ini_ctx, const char *filename);
static st_ini_t *st_ini_memload(st_modctx_t *ini_ctx, const void *ptr,
 size_t size);
static void st_ini_destroy(st_ini_t *ini);
static bool st_ini_section_exists(const st_ini_t *ini, const char *section);
static bool st_ini_key_exists(const st_ini_t *ini, const char *section,
 const char *key);
static const char *st_ini_get_str(const st_ini_t *ini, const char *section,
 const char *key);
static bool st_ini_fill_str(const st_ini_t *ini, char *dst, size_t dstsize,
 const char *section, const char *key);
static bool st_ini_delete_section(st_ini_t *ini, const char *section);
static bool st_ini_delete_key(st_ini_t *ini, const char *section,
 const char *key);
static bool st_ini_clear_section(st_ini_t *ini, const char *section);
static bool st_ini_add_section(st_ini_t *ini, const char *section);
static bool st_ini_add_key(st_ini_t *ini, const char *section, const char *key,
 const char *value);
static bool st_ini_export(const st_ini_t *ini, char *buffer, size_t bufsize);
static bool st_ini_save(const st_ini_t *ini, const char *filename);

#endif /* ST_STEROIDS_INI_H */
