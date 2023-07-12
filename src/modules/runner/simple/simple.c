#include "simple.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop

#define DEFAULT_CONFIG_FILENAME   "steroids.ini"
#define DEFAULT_DIRECTORY_NAME    "."
#define ERR_MSG_BUF_SIZE          1024
#define RUNNABLE_MODULE_NAME_SIZE 256

typedef st_modctx_t *(*runnable_init_func_t)(st_modctx_t *logger_ctx);
typedef void (*runnable_quit_func_t)(st_modctx_t *runnable_ctx);
typedef void (*runnable_run_func_t)(st_modctx_t *runner_ctx,
 const void *params);

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_runner_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_runner_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_runner_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"runner_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_runner_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_runner_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_runner_import_functions(st_modctx_t *runner_ctx,
 st_modctx_t *ini_ctx, st_modctx_t *logger_ctx, st_modctx_t *opts_ctx,
 st_modctx_t *plugin_ctx) {
    st_runner_simple_t *module       = runner_ctx->data;
    st_ini_funcs_t     *ini_funcs    = (st_ini_funcs_t *)ini_ctx->funcs;
    st_logger_funcs_t  *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;
    st_opts_funcs_t    *opts_funcs   = (st_opts_funcs_t *)opts_ctx->funcs;
    st_plugin_funcs_t  *plugin_funcs = (st_plugin_funcs_t *)plugin_ctx->funcs;

    module->ini.load        = ini_funcs->ini_load;
    module->ini.destroy     = ini_funcs->ini_destroy;
    module->ini.fill_str    = ini_funcs->ini_fill_str;

    module->logger.debug    = logger_funcs->logger_debug;
    module->logger.info     = logger_funcs->logger_info;
    module->logger.warning  = logger_funcs->logger_warning;
    module->logger.error    = logger_funcs->logger_error;

    module->opts.add_option = opts_funcs->opts_add_option;
    module->opts.get_str    = opts_funcs->opts_get_str;

    module->plugin.load     = plugin_funcs->plugin_load;
}

static st_modctx_t *st_runner_init(st_modctx_t *ini_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *opts_ctx, st_modctx_t *plugin_ctx) {
    st_modctx_t        *runner_ctx;
    st_runner_simple_t *module;

    runner_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_runner_simple_data, sizeof(st_runner_simple_t));
    if (!runner_ctx)
        return NULL;

    runner_ctx->funcs = &st_runner_simple_funcs;

    st_runner_import_functions(runner_ctx, ini_ctx, logger_ctx, opts_ctx,
     plugin_ctx);
    module = ini_ctx->data;
    module->ini.ctx    = ini_ctx;
    module->logger.ctx = logger_ctx;
    module->opts.ctx   = opts_ctx;
    module->plugin.ctx = plugin_ctx;

    module->logger.info(module->logger.ctx,
     "runner_simple: module initialized.");

    return runner_ctx;
}

static void st_runner_quit(st_modctx_t *runner_ctx) {
    st_runner_simple_t *module = runner_ctx->data;

    module->logger.info(module->logger.ctx, "runner_simple: module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, runner_ctx);
}

static bool get_config_filename(st_runner_simple_t *module,
 char filename[PATH_MAX]) {
    bool                opt_added = module->opts.add_option(module->opts.ctx,
     'c', "cfg", ST_OA_REQUIRED, "filename", "config file");
    char                config_filename[PATH_MAX];
    errno_t             err;

    if (opt_added) {
        if (module->opts.get_str(module->opts.ctx, "cfg", config_filename,
         PATH_MAX))
            return true;

        module->logger.warning(module->logger.ctx,
         "runner_simple: unable to get cmdline option for config filename. "
         "Using default config file \"%s\"", DEFAULT_CONFIG_FILENAME);
    } else {
        module->logger.warning(module->logger.ctx,
         "runner_simple: unable to set cmdline option for config filename. "
         "Using default config file \"%s\"", DEFAULT_CONFIG_FILENAME);
    }

    err = strcpy_s(config_filename, PATH_MAX, DEFAULT_CONFIG_FILENAME);

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        module->logger.error(module->logger.ctx,
         "ini_inih: Unable to copy default config filename");

        return false;
    }

    return true;
}

static bool get_directory_name(st_runner_simple_t *module,
 char dirname[PATH_MAX], const st_ini_t *ini) {
    if (module->opts.add_option(module->opts.ctx, 'p', "plugin-path",
     ST_OA_REQUIRED, "path", "Path where plugins stored")) {
        if (module->opts.get_str(module->opts.ctx, "plugin-path", dirname,
         PATH_MAX))
            return true;
    }

    if (ini && !module->ini.fill_str(ini, dirname, PATH_MAX, "steroids.runner",
     "plugin_path")) {
        errno_t err;

        module->logger.warning(module->logger.ctx,
         "runner_simple: unable to get plugin directory name. Using default "
         "directory \"%s\"", DEFAULT_DIRECTORY_NAME);

        err = strcpy_s(dirname, PATH_MAX, DEFAULT_DIRECTORY_NAME);
        if (err) {
            strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
            module->logger.error(module->logger.ctx,
             "runner_simple: unable to copy default plugin directory name "
             "\"%s\": %s", DEFAULT_DIRECTORY_NAME, err_msg_buf);

            return false;
        }
    }

    return !!ini;
}

static bool get_runnable_module_name(st_runner_simple_t *module,
 char runnable[RUNNABLE_MODULE_NAME_SIZE], const st_ini_t *ini) {
    if (module->opts.add_option(module->opts.ctx, 'r', "run", ST_OA_REQUIRED,
     "module_name", "Name of the mudule that must be launched")) {
        if (module->opts.get_str(module->opts.ctx, "run", runnable,
         RUNNABLE_MODULE_NAME_SIZE))
            return true;
    }

    if (ini && !module->ini.fill_str(ini, runnable, RUNNABLE_MODULE_NAME_SIZE,
     "steroids.runner", "run_module")) {
        module->logger.error(module->logger.ctx,
         "runner_simple: unable to get runnable module name");

        return false;
    }

    return !!ini;
}

static bool get_script_name(st_runner_simple_t *module,
 char script_name[PATH_MAX], const st_ini_t *ini) {
    if (module->opts.add_option(module->opts.ctx, 's', "script",
     ST_OA_REQUIRED, "filename", "Name of the script that must be launched")) {
        if (module->opts.get_str(module->opts.ctx, "script", script_name,
         PATH_MAX))
            return true;
    }

    if (ini && !module->ini.fill_str(ini, script_name, PATH_MAX,
     "steroids.runner", "script"))
        return false;

    return !!ini;
}

static bool load_plugins(st_runner_simple_t *module,
 const char dirname[PATH_MAX]) {
    struct dirent *entry;
    DIR           *dir = opendir(dirname);

    if (!dir) {
        module->logger.error(module->logger.ctx,
         "runner_simple: unable to open directory \"%s\": %s", dirname,
         strerror(errno));

        return false;
    }

    entry = readdir(dir);
    while (entry) {
        if (entry->d_type == DT_REG)
            module->plugin.load(module->plugin.ctx, entry->d_name);

        entry = readdir(dir);
    }

    return true;
}

static void run_runnable(st_runner_simple_t *module,
 const char *module_subsystem, const char *module_name,
 const char script_name[PATH_MAX]) {
    runnable_init_func_t runnable_init_func;
    runnable_quit_func_t runnable_quit_func;
    runnable_run_func_t  runnable_run_func;
    st_modctx_t         *runnable_ctx;

    runnable_init_func = global_modsmgr_funcs.get_function(global_modsmgr,
     module_subsystem, module_name, "init");
    runnable_quit_func = global_modsmgr_funcs.get_function(global_modsmgr,
     module_subsystem, module_name, "quit");
    runnable_run_func = global_modsmgr_funcs.get_function(global_modsmgr,
     module_subsystem, module_name, "run");


    if (!runnable_init_func || !runnable_quit_func || !runnable_run_func)
        return;

    runnable_ctx = runnable_init_func(module->logger.ctx);
    if (!runnable_ctx)
        return;

    runnable_run_func(runnable_ctx, script_name);
    runnable_quit_func(runnable_ctx);
}

static void st_runner_run(st_modctx_t *runner_ctx,
 __attribute__((unused)) const void *params) {
    st_runner_simple_t *module = runner_ctx->data;
    char                cfg_filename[PATH_MAX];
    st_ini_t           *ini;
    char                dirname[PATH_MAX];
    char                runnable[RUNNABLE_MODULE_NAME_SIZE];
    char                script_name[PATH_MAX] = "";
    char               *runnable_subsystem = runnable;
    char               *runnable_name;

    if (!get_config_filename(module, cfg_filename))
        return;

    ini = module->ini.load(module->ini.ctx, cfg_filename);

    if (!get_directory_name(module, dirname, ini) ||
     !get_runnable_module_name(module, runnable, ini))
        goto fail;

    runnable_name = strchr(runnable, ':');
    if (!runnable_name)
        goto fail;

    *runnable_name++ = '\0';

    get_script_name(module, script_name, ini);

    if (!load_plugins(module, dirname))
        goto fail;

    run_runnable(module, runnable_subsystem, runnable_name, script_name);

fail:
    if (ini)
        module->ini.destroy(ini);
}
