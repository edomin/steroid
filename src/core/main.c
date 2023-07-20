#include <stdbool.h>
#include <stdlib.h>

#include "modules_manager.h"

#include "steroids/types/modules/fs.h"
#include "steroids/types/modules/ini.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/opts.h"
#include "steroids/types/modules/pathtools.h"
#include "steroids/types/modules/plugin.h"
#include "steroids/types/modules/runner.h"
#include "steroids/types/modules/so.h"
#include "steroids/types/modules/spcpaths.h"
#include "steroids/types/modules/zip.h"

static st_fs_init_t st_fs_init;
static st_fs_quit_t st_fs_quit;

static st_ini_init_t st_ini_init;
static st_ini_quit_t st_ini_quit;

static st_logger_init_t  st_logger_init;
static st_logger_error_t st_logger_error;
static st_logger_quit_t  st_logger_quit;

static st_opts_init_t st_opts_init;
static st_opts_quit_t st_opts_quit;

static st_runner_init_t st_runner_init;
static st_runner_quit_t st_runner_quit;
static st_runner_run_t  st_runner_run;

static st_pathtools_init_t st_pathtools_init;
static st_pathtools_quit_t st_pathtools_quit;

static st_plugin_init_t st_plugin_init;
static st_plugin_quit_t st_plugin_quit;

static st_so_init_t st_so_init;
static st_so_quit_t st_so_quit;

static st_spcpaths_init_t st_spcpaths_init;
static st_spcpaths_quit_t st_spcpaths_quit;

static st_zip_init_t st_zip_init;
static st_zip_quit_t st_zip_quit;

#define LOAD_FUNCTION(module, function)                                        \
    st_##module##_##function = st_modsmgr_get_function(modsmgr, #module, NULL, \
     #function);                                                               \
    if (!st_##module##_##function) {                                           \
        st_logger_error(logger, "steroids: Unable to load function \"%s\"",    \
         #function);                                                           \
        return false;                                                          \
    }

static bool init_funcs(st_modsmgr_t *modsmgr, st_modctx_t *logger) {
    LOAD_FUNCTION(fs, init);
    LOAD_FUNCTION(fs, quit);

    LOAD_FUNCTION(ini, init);
    LOAD_FUNCTION(ini, quit);

    LOAD_FUNCTION(opts, init);
    LOAD_FUNCTION(opts, quit);

    LOAD_FUNCTION(runner, init);
    LOAD_FUNCTION(runner, quit);
    LOAD_FUNCTION(runner, run);

    LOAD_FUNCTION(pathtools, init);
    LOAD_FUNCTION(pathtools, quit);

    LOAD_FUNCTION(plugin, init);
    LOAD_FUNCTION(plugin, quit);

    LOAD_FUNCTION(so, init);
    LOAD_FUNCTION(so, quit);

    LOAD_FUNCTION(spcpaths, init);
    LOAD_FUNCTION(spcpaths, quit);

    LOAD_FUNCTION(zip, init);
    LOAD_FUNCTION(zip, quit);

    return true;
}

int main(int argc, char **argv) {
    st_modsmgr_t *modsmgr = st_modsmgr_init();
    st_modctx_t  *fs;
    st_modctx_t  *ini;
    st_modctx_t  *logger;
    st_modctx_t  *opts;
    st_modctx_t  *runner;
    st_modctx_t  *pathtools;
    st_modctx_t  *plugin;
    st_modctx_t  *so;
    st_modctx_t  *spcpaths;
    st_modctx_t  *zip;
    int           exitcode = EXIT_SUCCESS;

    st_logger_init  = st_modsmgr_get_function(modsmgr, "logger", NULL, "init");
    st_logger_quit  = st_modsmgr_get_function(modsmgr, "logger", NULL, "quit");
    st_logger_error = st_modsmgr_get_function(modsmgr, "logger", NULL, "error");
    logger = st_logger_init(NULL);

    if (!init_funcs(modsmgr, logger)) {
        exitcode = EXIT_FAILURE;

        goto init_funcs_fail;
    }

    ini = st_ini_init(logger);
    opts = st_opts_init(argc, argv, logger);
    pathtools = st_pathtools_init(logger);
    fs = st_fs_init(logger, pathtools);
    so = st_so_init(logger);
    spcpaths = st_spcpaths_init(logger);
    zip = st_zip_init(fs, logger, pathtools);
    plugin = st_plugin_init(fs, logger, pathtools, so, spcpaths, zip);
    runner = st_runner_init(ini, logger, opts, pathtools, plugin);

    st_runner_run(runner, NULL);

    st_runner_quit(runner);
    st_plugin_quit(plugin);
    st_zip_quit(zip);
    st_spcpaths_quit(spcpaths);
    st_so_quit(so);
    st_fs_quit(fs);
    st_pathtools_quit(pathtools);
    st_opts_quit(opts);
    st_ini_quit(ini);
init_funcs_fail:
    st_logger_quit(logger);

    st_modsmgr_destroy(modsmgr);

    return exitcode;
}
