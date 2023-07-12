#ifndef ST_STEROIDS_INI_H
#define ST_STEROIDS_INI_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/ini.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_ini_init(st_modctx_t *fnv1a_ctx,
 st_modctx_t *hash_table_ctx, st_modctx_t *logger_ctx);
ST_VISIBILITY void st_ini_quit(st_modctx_t *ini_ctx);

ST_VISIBILITY st_ini_t *st_ini_create(st_modctx_t *ini_ctx);
ST_VISIBILITY st_ini_t *st_ini_load(st_modctx_t *ini_ctx, const char *filename);
ST_VISIBILITY st_ini_t *st_ini_memload(st_modctx_t *ini_ctx, const void *ptr,
 size_t size);
ST_VISIBILITY void st_ini_destroy(st_ini_t *ini);
ST_VISIBILITY bool st_ini_section_exists(const st_ini_t *ini,
 const char *section);
ST_VISIBILITY bool st_ini_key_exists(const st_ini_t *ini, const char *section,
 const char *key);
ST_VISIBILITY const char *st_ini_get_str(const st_ini_t *ini,
 const char *section, const char *key);
ST_VISIBILITY bool st_ini_fill_str(const st_ini_t *ini, char *dst,
 size_t dstsize, const char *section, const char *key);
ST_VISIBILITY bool st_ini_delete_section(st_ini_t *ini, const char *section);
ST_VISIBILITY bool st_ini_delete_key(st_ini_t *ini, const char *section,
 const char *key);
ST_VISIBILITY bool st_ini_clear_section(st_ini_t *ini, const char *section);
ST_VISIBILITY bool st_ini_add_section(st_ini_t *ini, const char *section);
ST_VISIBILITY bool st_ini_add_key(st_ini_t *ini, const char *section,
 const char *key, const char *value);
ST_VISIBILITY bool st_ini_export(const st_ini_t *ini, char *buffer,
 size_t bufsize);
ST_VISIBILITY bool st_ini_save(const st_ini_t *ini, const char *filename);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_INI_H */
