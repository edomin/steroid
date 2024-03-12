#include "inih.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ini.h>

#define ERRMSGBUF_SIZE      128
#define SAVE_BUFFER_SIZE 131072

typedef struct {
    st_ini_t   *ini;
    const char *filename;
} st_userdata_t;

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

static void st_ini_free_section(void *ptr) {
    st_inisection_t *section = ptr;
    st_ini_inih_t   *module = section->module;

    module->htable.destroy(section->data);
    free(section);
}

ST_MODULE_DEF_GET_FUNC(ini_inih)
ST_MODULE_DEF_INIT_FUNC(ini_inih)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_ini_inih_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_ini_import_functions(st_modctx_t *ini_ctx,
 st_modctx_t *logger_ctx) {
    st_ini_inih_t *module = ini_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "ini_inih: Unable to load function \"error\" from module \"logger\"\n"
        );

        return false;
    }

    ST_LOAD_FUNCTION("ini_inih", fnv1a, NULL, get_u32hashstr_func);

    ST_LOAD_FUNCTION("ini_inih", htable, NULL, create);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, destroy);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, insert);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, get);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, remove);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, clear);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, contains);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, find);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, next);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, get_iter_key);
    ST_LOAD_FUNCTION("ini_inih", htable, NULL, get_iter_value);

    ST_LOAD_FUNCTION_FROM_CTX("ini_inih", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("ini_inih", logger, info);

    return true;
}

static st_modctx_t *st_ini_init(st_modctx_t *logger_ctx) {
    st_modctx_t   *ini_ctx;
    st_ini_inih_t *module;

    ini_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_ini_inih_data, sizeof(st_ini_inih_t));

    if (!ini_ctx)
        return NULL;

    module = ini_ctx->data;

    module->htable.init = global_modsmgr_funcs.get_function(global_modsmgr,
     "htable", NULL, "init");
    if (!module->htable.init) {
        fprintf(stderr,
         "ini_inih: Unable to load function \"init\" from module \"htable\"\n");

        goto get_func_fail;
    }

    module->htable.quit = global_modsmgr_funcs.get_function(global_modsmgr,
     "htable", NULL, "quit");
    if (!module->htable.quit) {
        fprintf(stderr,
         "ini_inih: Unable to load function \"quit\" from module \"htable\"\n");

        goto get_func_fail;
    }

    ini_ctx->funcs = &st_ini_inih_funcs;

    module->htable.ctx = module->htable.init(logger_ctx);
    if (!module->htable.ctx)
        goto htable_init_fail;

    module->logger.ctx = logger_ctx;

    if (!st_ini_import_functions(ini_ctx, logger_ctx))
        goto import_funcs_fail;

    module->logger.info(module->logger.ctx, "ini_inih: Module initialized.");

    return ini_ctx;

import_funcs_fail:
    module->htable.quit(module->htable.ctx);
htable_init_fail:
get_func_fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, ini_ctx);

    return NULL;
}

static void st_ini_quit(st_modctx_t *ini_ctx) {
    st_ini_inih_t *module = ini_ctx->data;

    module->htable.quit(module->htable.ctx);

    module->logger.info(module->logger.ctx, "ini_inih: Module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, ini_ctx);
}

static bool st_keyeqfunc(const void *left, const void *right) {
    return strcmp(left, right) == 0;
}

static st_ini_t *st_ini_create(st_modctx_t *ini_ctx) {
    st_ini_inih_t *module = ini_ctx->data;
    st_ini_t      *ini = malloc(sizeof(st_ini_t));

    if (!ini) {
        char errbuf[ERRMSGBUF_SIZE];

        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to allocate memory for ini: %s", errbuf);

        return NULL;
    }

    ini->module = module;
    ini->sections = module->htable.create(module->htable.ctx,
     (unsigned int (*)(const void *))module->fnv1a.get_u32hashstr_func(NULL),
     st_keyeqfunc, free, st_ini_free_section);
    if (!ini->sections) {
        module->logger.error(module->logger.ctx,
         "ini_inih: Unable to create hash table for sections");

        free(ini);
        return NULL;
    }

    return ini;
}

#if INI_HANDLER_LINENO
static int st_ini_parse_handler(void *userdata, const char *section,
 const char *key, const char *value, int lineno) {
#else
static int st_ini_parse_handler(void *userdata, const char *section,
 const char *key, const char *value) {
#endif
    st_userdata_t *typed_userdata = userdata;
    st_ini_t      *ini = typed_userdata->ini;

    if (!key && !value)
        return st_ini_add_section(ini, section) ? 1 : 0;

    if (!key || !value)
        return 0;

    return st_ini_add_key(ini, section, key, value) ? 1 : 0;
}

static void st_process_error(int parse_result, const char *filename,
 st_ini_inih_logger_t *logger) {
    switch (parse_result) {
        case 0:
            break;
        case -1:
            logger->error(logger->ctx,
             "ini_inih: Unable to open file \"%s\"", filename);
            break;
        case -2:
            logger->error(logger->ctx,
             "ini_inih: Memory allocation error while parsing file \"%s\"",
             filename);
            break;
        default:
            logger->error(logger->ctx, "ini_inih: Syntax error at %s:%d",
             filename, parse_result);
            break;
    }
}

static st_ini_t *st_ini_load(st_modctx_t *ini_ctx, const char *filename) {
    st_ini_inih_t *module = ini_ctx->data;
    st_userdata_t  userdata;
    st_ini_t      *ini = st_ini_create(ini_ctx);
    int            ret;

    if (!ini)
        return NULL;

    userdata.ini = ini;
    userdata.filename = filename;
    ret = ini_parse(filename, st_ini_parse_handler, &userdata);
    if (ret != 0) {
        st_process_error(ret, filename, &module->logger);
        st_ini_destroy(ini);

        return NULL;
    }

    return ini;
}

static st_ini_t *st_ini_memload(st_modctx_t *ini_ctx, const void *ptr,
 size_t size) {
    st_ini_inih_t *module = ini_ctx->data;
    st_ini_t      *ini = st_ini_create(ini_ctx);
    int            ret;
    char          *zero_terminated;

    if (!ini)
        return NULL;

    if (size == 0) {
        ret = ini_parse_string(ptr, st_ini_parse_handler, ini);
        if (ret != 0) {
            st_process_error(ret, NULL, &module->logger);

            goto ini_destroy;
        }
    } else {
        zero_terminated = malloc(size + 1);

        if (!zero_terminated) {
            char errbuf[ERRMSGBUF_SIZE];

            if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
                module->logger.error(module->logger.ctx,
                 "ini_inih: Unable to allocate memory for temporary ini "
                 "buffer: %s", errbuf);

            goto ini_destroy;
        }

        ret = snprintf(zero_terminated, size + 1, "%s", (const char *)ptr);
        if (ret < 0 || (size_t)ret == size + 1) {
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to copy ini data to temp buffer");

            goto free_terminated_and_ini_destroy;
        }

        ret = ini_parse_string(zero_terminated, st_ini_parse_handler, ini);
        if (ret != 0) {
            st_process_error(ret, NULL, &module->logger);
            goto free_terminated_and_ini_destroy;
        }

        free(zero_terminated);
    }

    return ini;

free_terminated_and_ini_destroy:
    free(zero_terminated);
ini_destroy:
    st_ini_destroy(ini);
    return NULL;
}

static void st_ini_destroy(st_ini_t *ini) {
    st_ini_inih_t *module = ini->module;

    module->htable.destroy(ini->sections);
    free(ini);
}

static bool st_ini_section_exists(const st_ini_t *ini, const char *section) {
    st_ini_inih_t *module = ini->module;

    return !!module->htable.get(ini->sections, !!section ? section : "");
}

static bool st_ini_key_exists(const st_ini_t *ini, const char *section,
 const char *key) {
    return !!st_ini_get_str(ini, section, key);
}

static const char *st_ini_get_str(const st_ini_t *ini, const char *section_name,
 const char *key) {
    st_ini_inih_t *module = ini->module;
    st_inisection_t *section = module->htable.get(ini->sections,
     !!section_name ? section_name : "");

    if (!section)
        return false;

    return module->htable.get(section->data, key);
}

static bool st_ini_fill_str(const st_ini_t *ini, char *dst, size_t dstsize,
 const char *section_name, const char *key) {
    const char *str = st_ini_get_str(ini, section_name, key);
    int         ret;

    if (!str)
        return false;

    ret = snprintf(dst, dstsize, "%s", str);

    return ret > 0 && (size_t)ret < dstsize;
}

static bool st_ini_delete_section(st_ini_t *ini, const char *section) {
    st_ini_inih_t *module = ini->module;

    return module->htable.remove(ini->sections, section);
}

static bool st_ini_delete_key(st_ini_t *ini, const char *section_name,
 const char *key) {
    st_ini_inih_t   *module = ini->module;
    st_htiter_t      section_iter;
    st_inisection_t *section;

    if (!module->htable.find(ini->sections, &section_iter, section_name))
        return false;

    section = module->htable.get_iter_value(&section_iter);

    return module->htable.remove(section->data, key);
}

static bool st_ini_clear_section(st_ini_t *ini, const char *section_name) {
    st_ini_inih_t   *module = ini->module;
    st_htiter_t      section_iter;
    st_inisection_t *section;

    if (!module->htable.find(ini->sections, &section_iter, section_name))
        return false;

    section = module->htable.get_iter_value(&section_iter);
    module->htable.clear(section->data);

    return true;
}

static bool st_ini_add_section(st_ini_t *ini, const char *section_name) {
    st_ini_inih_t   *module = ini->module;
    st_inisection_t *section;
    char            *section_key;
    char             errbuf[ERRMSGBUF_SIZE];

    if (module->htable.contains(ini->sections, section_name))
        return true;

    st_htable_t *section_ht = module->htable.create(
     module->htable.ctx,
     (unsigned int (*)(const void *))module->fnv1a.get_u32hashstr_func(NULL),
     st_keyeqfunc, free, free);
    if (!section_ht) {
        module->logger.error(module->logger.ctx,
         "ini_inih: Unable to create section \"%s\"", section_name);

        return false;
    }

    section = malloc(sizeof(st_inisection_t));
    if (!section) {
        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to allocate memory for section \"%s\": %s",
             section_name, errbuf);

        goto malloc_fail;
    }
    section->data = section_ht;
    section->module = module;

    section_key = strdup(section_name);
    if (!section_key) {
        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to allocate memory for section key: %s", errbuf);

        goto strdup_fail;
    }

    if (!module->htable.insert(ini->sections, NULL, section_key,
     section)) {
        module->logger.error(module->logger.ctx,
         "ini_inih: Unable to insert unnamed section to sections table");

        goto insert_fail;
    }

    return true;

insert_fail:
    free(section_key);
strdup_fail:
    free(section);
malloc_fail:
    module->htable.destroy(section_ht);

    return false;
}

static bool st_ini_add_key(st_ini_t *ini, const char *section_name,
 const char *key, const char *value) {
    st_ini_inih_t   *module = ini->module;
    st_inisection_t *section;
    char            *keydup;
    char            *valdup;
    char             errbuf[ERRMSGBUF_SIZE];

    if (!st_ini_add_section(ini, section_name))
        return false;

    section = module->htable.get(ini->sections, section_name);

    keydup = strdup(key);
    if (!keydup) {
        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to allocate memory for key: %s", errbuf);

        return false;
    }

    valdup = strdup(value);
    if (!valdup) {
        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to allocate memory for value: %s", errbuf);

        goto valdup_fail;
    }

    if (!module->htable.insert(section->data, NULL, keydup, valdup)) {
        module->logger.error(module->logger.ctx,
         "ini_inih: Unable to insert key \"%s\" to section \"%s\"", key,
         section_name);

        goto insert_fail;
    }

    return true;

insert_fail:
    free(valdup);
valdup_fail:
    free(keydup);

    return false;
}

static bool st_ini_export(const st_ini_t *ini, char *buffer, size_t bufsize) {
    st_ini_inih_t *module = ini->module;
    st_htiter_t    section_it;
    size_t         bufoffset;

    if (!module->htable.next(ini->sections, &section_it, NULL))
        return true;

    do {
        const char      *sec_key = module->htable.get_iter_key(&section_it);
        st_inisection_t *section = module->htable.get_iter_value(&section_it);
        st_htiter_t      key_it;
        int              sec_ret = 0;

        if (sec_key[0] != '\n') { // if section is not unnamed
            sec_ret = snprintf(buffer, bufsize, "[%s]\n", sec_key);
            if (sec_ret < 0 || (size_t)sec_ret == bufsize) {
                module->logger.error(module->logger.ctx,
                 "ini_inih: Unable to construct section header for section: %s",
                 sec_key);

                return NULL;
            }
        }

        buffer += sec_ret;
        bufsize -= (size_t)sec_ret;

        if (module->htable.next(section->data, &key_it, NULL))
            continue;

        do {
            const char *key = module->htable.get_iter_key(&key_it);
            char       *value = module->htable.get_iter_value(&key_it);
            int         ret = snprintf(buffer, bufsize, "%s=%s\n", key, value);

            if (ret < 0 || (size_t)ret == bufsize) {
                module->logger.error(module->logger.ctx,
                 "ini_inih: Unable to construct key-value record for key: %s",
                 key);

                return NULL;
            }

            buffer += ret;
            bufsize -= (size_t)ret;
        } while (module->htable.next(section->data, &key_it, &key_it));

        if (bufsize-- == 0) {
            module->logger.error(module->logger.ctx,
             "ini_inih: Buffer overflow while trying to export section \"%s\"",
             sec_key);

            return NULL;
        }
        *buffer++ = '\n';
    } while (module->htable.next(ini->sections, &section_it, &section_it));

    buffer[bufsize - 1] = '\n';

    return true;
}

static bool st_ini_save(const st_ini_t *ini, const char *filename) {
    st_ini_inih_t *module = ini->module;
    char           buffer[SAVE_BUFFER_SIZE];
    bool           buffer_filled = st_ini_export(ini, buffer, SAVE_BUFFER_SIZE);
    FILE          *file;
    size_t         buflen;
    size_t         writen;

    if (!buffer_filled)
        return false;

    file = fopen(filename, "wbe");

    if (!file) {
        char errbuf[ERRMSGBUF_SIZE];

        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to open file \"%s\": %s\n", filename, errbuf);

        return false;
    }

    buflen = strlen(buffer);
    writen = fwrite(buffer, 1, buflen, file);

    fclose(file);

    return writen == buflen;
}
