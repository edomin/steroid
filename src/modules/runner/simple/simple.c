#include "simple.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_CONFIG_FILENAME   "steroids.ini"
#define DEFAULT_DIRECTORY_NAME    "."
#define RUNNABLE_MODULE_NAME_SIZE 256

typedef st_modctx_t *(*runnable_init_func_t)(st_modctx_t *logger_ctx,
 st_modctx_t *opts_ctx);
typedef void (*runnable_quit_func_t)(st_modctx_t *runnable_ctx);
typedef void (*runnable_run_func_t)(st_modctx_t *runner_ctx,
 const void *params);

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(runner_simple)
ST_MODULE_DEF_INIT_FUNC(runner_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_runner_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_runner_import_functions(st_modctx_t *runner_ctx,
 st_modctx_t *ini_ctx, st_modctx_t *logger_ctx, st_modctx_t *opts_ctx,
 st_modctx_t *pathtools_ctx, st_modctx_t *plugin_ctx) {
    st_runner_simple_t *module = runner_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function(global_modsmgr,
     "logger", NULL, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "runner_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", ini, load);
    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", ini, destroy);
    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", ini, fill_str);

    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", logger, info);
    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", logger, warning);

    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", opts, add_option);
    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", opts, get_str);

    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", pathtools, concat);

    ST_LOAD_FUNCTION_FROM_CTX("runner_simple", plugin, load);

    return true;
}

static st_modctx_t *st_runner_init(st_modctx_t *ini_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *opts_ctx, st_modctx_t *pathtools_ctx,
 st_modctx_t *plugin_ctx) {
    st_modctx_t        *runner_ctx;
    st_runner_simple_t *module;

    runner_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_runner_simple_data, sizeof(st_runner_simple_t));
    if (!runner_ctx)
        return NULL;

    runner_ctx->funcs = &st_runner_simple_funcs;

    module = runner_ctx->data;
    module->ini.ctx       = ini_ctx;
    module->logger.ctx    = logger_ctx;
    module->opts.ctx      = opts_ctx;
    module->pathtools.ctx = pathtools_ctx;
    module->plugin.ctx    = plugin_ctx;

    if (!st_runner_import_functions(runner_ctx, ini_ctx, logger_ctx, opts_ctx,
     pathtools_ctx, plugin_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, runner_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx,
     "runner_simple: Runner initialized.");

    return runner_ctx;
}

static void st_runner_quit(st_modctx_t *runner_ctx) {
    st_runner_simple_t *module = runner_ctx->data;

    module->logger.info(module->logger.ctx, "runner_simple: Runner destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, runner_ctx);
}

static bool get_config_filename(st_runner_simple_t *module,
 char filename[PATH_MAX]) {
    int ret;

    if (module->opts.add_option(module->opts.ctx, 'c', "cfg", ST_OA_REQUIRED,
     "filename", "Config file")) {
        if (module->opts.get_str(module->opts.ctx, "cfg", filename, PATH_MAX))
            return true;

        module->logger.warning(module->logger.ctx,
         "runner_simple: Unable to get cmdline option for config filename. "
         "Using default config file \"%s\"", DEFAULT_CONFIG_FILENAME);
    } else {
        module->logger.warning(module->logger.ctx,
         "runner_simple: Unable to set cmdline option for config filename. "
         "Using default config file \"%s\"", DEFAULT_CONFIG_FILENAME);
    }

    ret = snprintf(filename, PATH_MAX, "%s", DEFAULT_CONFIG_FILENAME);
    if (ret < 0 || ret == PATH_MAX) {
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
        int ret;

        module->logger.warning(module->logger.ctx,
         "runner_simple: Unable to get plugin directory name. Using default "
         "directory \"%s\"", DEFAULT_DIRECTORY_NAME);

        ret = snprintf(dirname, PATH_MAX, "%s", DEFAULT_DIRECTORY_NAME);
        if (ret < 0 || ret == PATH_MAX) {
            module->logger.error(module->logger.ctx,
             "runner_simple: Unable to copy default plugin directory name "
             "\"%s\"", DEFAULT_DIRECTORY_NAME);

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
         "runner_simple: Unable to get runnable module name");

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
         "runner_simple: Unable to open directory \"%s\": %s", dirname,
         strerror(errno));

        return false;
    }

    entry = readdir(dir);
    while (entry) {
        if (entry->d_type == DT_REG) {
            char filename[PATH_MAX];

            if (module->pathtools.concat(module->pathtools.ctx, filename,
             PATH_MAX, dirname, entry->d_name)) {
                if (!module->plugin.load(module->plugin.ctx, filename, true))
                    module->logger.error(module->logger.ctx,
                     "runner_simple: Unable to load plugin \"%s\"", filename);
            }
        }

        entry = readdir(dir);
    }

    global_modsmgr_funcs.process_deps(global_modsmgr);

    closedir(dir);

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
    if (!runnable_init_func) {
        module->logger.error(module->logger.ctx,
         "runner_simple: Unable to load function \"init\" from module "
         "\"%s_%s\"", module_subsystem, module_name);

        return;
    }

    runnable_quit_func = global_modsmgr_funcs.get_function(global_modsmgr,
     module_subsystem, module_name, "quit");
    if (!runnable_init_func) {
        module->logger.error(module->logger.ctx,
         "runner_simple: Unable to load function \"quit\" from module "
         "\"%s_%s\"", module_subsystem, module_name);

        return;
    }

    runnable_run_func = global_modsmgr_funcs.get_function(global_modsmgr,
     module_subsystem, module_name, "run");
    if (!runnable_init_func) {
        module->logger.error(module->logger.ctx,
         "runner_simple: Unable to load function \"run\" from module "
         "\"%s_%s\"", module_subsystem, module_name);

        return;
    }

    runnable_ctx = runnable_init_func(module->logger.ctx, module->opts.ctx);
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
    if (!runnable_name) {
        module->logger.error(module->logger.ctx,
         "runner_simple: Missing name of runnable module");

        goto fail;
    }

    *runnable_name++ = '\0';

    get_script_name(module, script_name, ini);

    if (!load_plugins(module, dirname))
        goto fail;

    run_runnable(module, runnable_subsystem, runnable_name, script_name);

fail:
    if (ini)
        module->ini.destroy(ini);
}
