#include "inih.h"

#include <ini.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop

#define ERR_MSG_BUF_SIZE 1024
#define SAVE_BUFFER_SIZE 131072

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

static void st_ini_free_section(void *ptr) {
    st_inisection_t *section = ptr;
    st_ini_inih_t   *module = section->module;

    module->hash_table.clear(section->data);
    free(section);
}

void *st_module_ini_inih_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table =
     &st_module_ini_inih_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_ini_inih_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    global_modsmgr = modsmgr;
    if (memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t)) != 0) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
        fprintf(stderr, "Unable to init module \"ini_inih\": %s\n",
         err_msg_buf);

        return NULL;
    }

    return &st_module_ini_inih_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_ini_inih_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_ini_import_functions(st_modctx_t *ini_ctx,
 st_modctx_t *fnv1a_ctx, st_modctx_t *hash_table_ctx, st_modctx_t *logger_ctx) {
    st_ini_inih_t         *ini_module = ini_ctx->data;
    st_fnv1a_funcs_t      *fnv1a_funcs = (st_fnv1a_funcs_t *)fnv1a_ctx->funcs;
    st_hash_table_funcs_t *ht_funcs =
     (st_hash_table_funcs_t *)hash_table_ctx->funcs;
    st_logger_funcs_t     *logger_funcs =
     (st_logger_funcs_t *)logger_ctx->funcs;

    ini_module->fnv1a.get_u32hashstr_func = fnv1a_funcs->fnv1a_get_u32hashstr_func;

    ini_module->hash_table.create         = ht_funcs->hash_table_create;
    ini_module->hash_table.destroy        = ht_funcs->hash_table_destroy;
    ini_module->hash_table.insert         = ht_funcs->hash_table_insert;
    ini_module->hash_table.get            = ht_funcs->hash_table_get;
    ini_module->hash_table.remove         = ht_funcs->hash_table_remove;
    ini_module->hash_table.clear          = ht_funcs->hash_table_clear;
    ini_module->hash_table.contains       = ht_funcs->hash_table_contains;
    ini_module->hash_table.find           = ht_funcs->hash_table_find;
    ini_module->hash_table.next           = ht_funcs->hash_table_next;
    ini_module->hash_table.get_iter_key   = ht_funcs->hash_table_get_iter_key;
    ini_module->hash_table.get_iter_value = ht_funcs->hash_table_get_iter_value;

    ini_module->logger.debug = logger_funcs->logger_debug;
    ini_module->logger.info  = logger_funcs->logger_info;
    ini_module->logger.error = logger_funcs->logger_error;
}

static st_modctx_t *st_ini_init(st_modctx_t *fnv1a_ctx,
 st_modctx_t *hash_table_ctx, st_modctx_t *logger_ctx) {
    st_modctx_t   *ini_ctx;
    st_ini_inih_t *module;

    ini_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_ini_inih_data, sizeof(st_ini_inih_t));

    if (ini_ctx == NULL)
        return NULL;

    ini_ctx->funcs = &st_ini_inih_funcs;

    st_ini_import_functions(ini_ctx, fnv1a_ctx, hash_table_ctx, logger_ctx);
    module = ini_ctx->data;
    module->fnv1a.ctx      = fnv1a_ctx;
    module->hash_table.ctx = hash_table_ctx;
    module->logger.ctx     = logger_ctx;

    module->logger.info(module->logger.ctx, "ini_inih: module initialized.");

    return ini_ctx;
}

static void st_ini_quit(st_modctx_t *ini_ctx) {
    st_ini_inih_t *module = ini_ctx->data;

    module->logger.info(module->logger.ctx, "ini_inih: module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, ini_ctx);
}

static bool st_keyeqfunc(const void *left, const void *right) {
    return strcmp(left, right) == 0;
}

static st_ini_t *st_ini_create(st_modctx_t *ini_ctx) {
    st_ini_inih_t *module = ini_ctx->data;
    st_ini_t      *ini = malloc(sizeof(st_ini_t));

    if (!ini) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to allocate memory for ini: %s", strerror(errno));

        return NULL;
    }

    ini->module = module;
    ini->sections = module->hash_table.create(module->hash_table.ctx,
     (unsigned int (*)(const void *))module->fnv1a.get_u32hashstr_func(
      module->fnv1a.ctx),
     st_keyeqfunc, free, st_ini_free_section);
    if (!ini->sections) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to create hashtable for sections");

        free(ini);
        return NULL;
    }

    return ini;
}

static int st_ini_parse_handler(void *userdata, const char *section,
 const char *key, const char *value) {
    st_ini_t *ini = userdata;

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
             "ini_inih: unable to open file \"%s\"", filename);
            break;
        case -2:
            logger->error(logger->ctx,
             "ini_inih: memory allocation error while parsing file \"%s\"",
             filename);
            break;
        default:
            logger->error(logger->ctx, "ini_inih: syntax error at %s:%d",
             filename, parse_result);
            break;
    }
}

static st_ini_t *st_ini_load(st_modctx_t *ini_ctx, const char *filename) {
    st_ini_inih_t *module = ini_ctx->data;
    st_ini_t      *ini = st_ini_create(ini_ctx);
    int            ret;

    if (!ini)
        return NULL;

    ret = ini_parse(filename, st_ini_parse_handler, ini);
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
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to allocate memory for temporary ini buffer: %s",
             strerror);
            goto ini_destroy;
        }

        if (strncpy_s(zero_terminated, size + 1, ptr, size + 1) != 0) {
            strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
            module->logger.error(module->logger.ctx,
             "ini_inih: Unable to copy ini data to temp buffer: %s",
             err_msg_buf);
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
    module->hash_table.clear(ini->sections);
    free(ini);
}

static bool st_ini_section_exists(const st_ini_t *ini, const char *section) {
    st_ini_inih_t *module = ini->module;

    return !!module->hash_table.get(ini->sections, !!section ? section : "");
}

static bool st_ini_key_exists(const st_ini_t *ini, const char *section,
 const char *key) {
    return !!st_ini_get_str(ini, section, key);
}

static const char *st_ini_get_str(const st_ini_t *ini, const char *section_name,
 const char *key) {
    st_ini_inih_t *module = ini->module;
    st_inisection_t *section = module->hash_table.get(ini->sections,
     !!section_name ? section_name : "");

    if (!section)
        return false;

    return module->hash_table.get(section->data, key);
}

static bool st_ini_delete_section(st_ini_t *ini, const char *section) {
    st_ini_inih_t *module = ini->module;

    return module->hash_table.remove(ini->sections, section);
}

static bool st_ini_delete_key(st_ini_t *ini, const char *section_name,
 const char *key) {
    st_ini_inih_t   *module = ini->module;
    st_htiter_t      section_iter;
    st_inisection_t *section;

    if (!module->hash_table.find(ini->sections, &section_iter, section_name))
        return false;

    section = module->hash_table.get_iter_value(&section_iter);

    return module->hash_table.remove(section->data, key);
}

static bool st_ini_clear_section(st_ini_t *ini, const char *section_name) {
    st_ini_inih_t   *module = ini->module;
    st_htiter_t      section_iter;
    st_inisection_t *section;

    if (!module->hash_table.find(ini->sections, &section_iter, section_name))
        return false;

    section = module->hash_table.get_iter_value(&section_iter);
    module->hash_table.clear(section->data);

    return true;
}

static bool st_ini_add_section(st_ini_t *ini, const char *section_name) {
    st_ini_inih_t *module = ini->module;
    st_inisection_t  *section;
    char          *section_key;

    if (module->hash_table.contains(ini->sections, section_name))
        return true;

    st_hashtable_t *section_ht = module->hash_table.create(
     module->hash_table.ctx,
     (unsigned int (*)(const void *))module->fnv1a.get_u32hashstr_func(
      module->fnv1a.ctx),
     st_keyeqfunc, free, free);
    if (!section_ht) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to create section \"%s\"", section_name);

        return false;
    }

    section = malloc(sizeof(st_inisection_t));
    if (!section) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to allocate memory for section \"%s\": %s",
         section_name, strerror(errno));

        goto malloc_fail;
    }
    section->data = section_ht;
    section->module = module;

    section_key = strdup(section_name);
    if (!section_key) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to allocate memory for section key: %s",
         strerror(errno));

        goto strdup_fail;
    }

    if (!module->hash_table.insert(ini->sections, NULL, section_key,
     section)) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to insert unnamed section to sections table");

        goto insert_fail;
    }

    return true;

insert_fail:
    free(section_key);
strdup_fail:
    free(section);
malloc_fail:
    module->hash_table.destroy(section_ht);

    return false;
}

static bool st_ini_add_key(st_ini_t *ini, const char *section_name,
 const char *key, const char *value) {
    st_ini_inih_t *module = ini->module;
    st_inisection_t  *section;
    char          *keydup;
    char          *valdup;

    if (!st_ini_add_section(ini, section_name))
        return false;

    section = module->hash_table.get(ini->sections, section_name);

    keydup = strdup(key);
    if (!keydup) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to allocate memory for key: %s", strerror(errno));

        return false;
    }

    valdup = strdup(value);
    if (!valdup) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to allocate memory for value: %s", strerror(errno));

        goto valdup_fail;
    }

    if (!module->hash_table.insert(section->data, NULL, keydup, valdup)) {
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to insert key \"%s\" to section \"%s\"", key,
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

    if (!module->hash_table.next(ini->sections, &section_it, NULL))
        return true;

    do {
        const char      *sec_key = module->hash_table.get_iter_key(&section_it);
        st_inisection_t *section = module->hash_table.get_iter_value(&section_it);
        st_htiter_t      key_it;
        int              sec_ret = 0;

        if (sec_key[0] != '\n') { // if section is not unnamed
            sec_ret = snprintf_s(buffer, bufsize, "[%s]\n", sec_key);
            if (sec_ret < 0) {
                strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
                module->logger.error(module->logger.ctx,
                 "ini_inih: snprintf_s: %s\n", err_msg_buf);

                return NULL;
            }
        }

        buffer += sec_ret;
        bufsize -= (size_t)sec_ret;

        if (module->hash_table.next(section->data, &key_it, NULL))
            continue;

        do {
            const char *key = module->hash_table.get_iter_key(&key_it);
            char       *value = module->hash_table.get_iter_value(&key_it);
            int         ret = snprintf_s(buffer, bufsize, "%s=%s\n", key, value);

            if (ret < 0) {
                strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
                module->logger.error(module->logger.ctx,
                 "ini_inih: snprintf_s: %s\n", err_msg_buf);

                return NULL;
            }

            buffer += ret;
            bufsize -= (size_t)ret;
        } while (module->hash_table.next(section->data, &key_it, &key_it));

        snprintf_s(buffer, bufsize, "\n");
        buffer++;
        bufsize--;
    } while (module->hash_table.next(ini->sections, &section_it, &section_it));

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
        module->logger.error(module->logger.ctx,
         "ini_inih: unable to open file \"%s\": %s\n", filename, err_msg_buf);

        return false;
    }

    buflen = strlen(buffer);
    writen = fwrite(buffer, 1, buflen, file);

    fclose(file);

    return writen == buflen;
}
