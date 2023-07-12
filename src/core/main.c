#include <stdbool.h>
#include <stdlib.h>

#include "modules_manager.h"

#include "steroids/types/modules/fnv1a.h"
#include "steroids/types/modules/fs.h"
#include "steroids/types/modules/hash_table.h"
#include "steroids/types/modules/ini.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/opts.h"
#include "steroids/types/modules/pathtools.h"
#include "steroids/types/modules/plugin.h"
#include "steroids/types/modules/runner.h"
#include "steroids/types/modules/so.h"
#include "steroids/types/modules/spcpaths.h"
#include "steroids/types/modules/zip.h"

static st_fnv1a_init_t st_fnv1a_init;
static st_fnv1a_quit_t st_fnv1a_quit;

static st_fs_init_t st_fs_init;
static st_fs_quit_t st_fs_quit;

static st_hash_table_init_t st_hash_table_init;
static st_hash_table_quit_t st_hash_table_quit;

static st_ini_init_t st_ini_init;
static st_ini_quit_t st_ini_quit;

static st_logger_init_t st_logger_init;
static st_logger_quit_t st_logger_quit;

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

static bool init_funcs(st_modsmgr_t *modsmgr) {
    // TODO(edomin): check getted functions

    st_fnv1a_init = st_modsmgr_get_function(modsmgr, "fnv1a", NULL,
     "st_fnv1a_init");
    st_fnv1a_quit = st_modsmgr_get_function(modsmgr, "fnv1a", NULL,
     "st_fnv1a_quit");

    st_fs_init = st_modsmgr_get_function(modsmgr, "fs", NULL, "st_fs_init");
    st_fs_quit = st_modsmgr_get_function(modsmgr, "fs", NULL, "st_fs_quit");

    st_hash_table_init = st_modsmgr_get_function(modsmgr, "hash_table", NULL,
     "st_hash_table_init");
    st_hash_table_quit = st_modsmgr_get_function(modsmgr, "hash_table", NULL,
     "st_hash_table_quit");

    st_ini_init = st_modsmgr_get_function(modsmgr, "ini", NULL, "st_ini_init");
    st_ini_quit = st_modsmgr_get_function(modsmgr, "ini", NULL, "st_ini_quit");

    st_logger_init = st_modsmgr_get_function(modsmgr, "logger", NULL,
     "st_logger_init");
    st_logger_quit = st_modsmgr_get_function(modsmgr, "logger", NULL,
     "st_logger_quit");

    st_opts_init = st_modsmgr_get_function(modsmgr, "opts", NULL,
     "st_opts_init");
    st_opts_quit = st_modsmgr_get_function(modsmgr, "opts", NULL,
     "st_opts_quit");

    st_runner_init = st_modsmgr_get_function(modsmgr, "runner", NULL,
     "st_runner_init");
    st_runner_quit = st_modsmgr_get_function(modsmgr, "runner", NULL,
     "st_runner_quit");
    st_runner_run = st_modsmgr_get_function(modsmgr, "runner", NULL,
     "st_runner_run");

    st_pathtools_init = st_modsmgr_get_function(modsmgr, "pathtools", NULL,
     "st_pathtools_init");
    st_pathtools_quit = st_modsmgr_get_function(modsmgr, "pathtools", NULL,
     "st_pathtools_quit");

    st_plugin_init = st_modsmgr_get_function(modsmgr, "plugin", NULL,
     "st_plugin_init");
    st_plugin_quit = st_modsmgr_get_function(modsmgr, "plugin", NULL,
     "st_plugin_quit");

    st_so_init = st_modsmgr_get_function(modsmgr, "so", NULL, "st_so_init");
    st_so_quit = st_modsmgr_get_function(modsmgr, "so", NULL, "st_so_quit");

    st_spcpaths_init = st_modsmgr_get_function(modsmgr, "spcpaths", NULL,
     "st_spcpaths_init");
    st_spcpaths_quit = st_modsmgr_get_function(modsmgr, "spcpaths", NULL,
     "st_spcpaths_quit");

    st_zip_init = st_modsmgr_get_function(modsmgr, "zip", NULL, "st_zip_init");
    st_zip_quit = st_modsmgr_get_function(modsmgr, "zip", NULL, "st_zip_quit");

    return st_fnv1a_init && st_fnv1a_quit && st_fs_init && st_fs_quit &&
     st_hash_table_init && st_hash_table_quit && st_ini_init && st_ini_quit &&
     st_logger_init && st_logger_quit && st_opts_init && st_opts_quit &&
     st_runner_init && st_runner_quit && st_runner_run && st_pathtools_init &&
     st_pathtools_quit && st_plugin_init && st_plugin_quit && st_so_init &&
     st_so_quit && st_spcpaths_init && st_spcpaths_quit && st_zip_init &&
     st_zip_quit;
}

int main(int argc, char **argv) {
    st_modsmgr_t *modsmgr = st_modsmgr_init();
    st_modctx_t  *fnv1a;
    st_modctx_t  *fs;
    st_modctx_t  *hash_table;
    st_modctx_t  *ini;
    st_modctx_t  *logger;
    st_modctx_t  *opts;
    st_modctx_t  *runner;
    st_modctx_t  *pathtools;
    st_modctx_t  *plugin;
    st_modctx_t  *so;
    st_modctx_t  *spcpaths;
    st_modctx_t  *zip;

    if (init_funcs(modsmgr))
        return EXIT_FAILURE;

    logger = st_logger_init();
    fnv1a = st_fnv1a_init(logger);
    hash_table = st_hash_table_init(logger);
    ini = st_ini_init(fnv1a, hash_table, logger);
    opts = st_opts_init(argc, argv, logger);
    pathtools = st_pathtools_init(logger);
    fs = st_fs_init(logger, pathtools);
    so = st_so_init(logger);
    spcpaths = st_spcpaths_init(logger);
    zip = st_zip_init(fs, logger, pathtools);
    plugin = st_plugin_init(fs, logger, pathtools, so, spcpaths, zip);
    runner = st_runner_init(ini, logger, opts, plugin);

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
    st_hash_table_quit(hash_table);
    st_fnv1a_quit(fnv1a);
    st_logger_quit(logger);

    st_modsmgr_destroy(modsmgr);

    return EXIT_SUCCESS;
}
