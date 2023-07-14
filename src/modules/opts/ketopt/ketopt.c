#include "ketopt.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define LONG_OPT_NUM_TO_INDEX_OFFSET 300
#define SHORTOPT_FMT_SIZE            3
#define SHORT_OPTS_FMT_SIZE          100
#define LONGOPTS_COUNT               64
#define ERR_MSG_BUF_SIZE             1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

typedef enum {
    ST_OT_SHORT,
    ST_OT_LONG,
} st_opttype_t;

void *st_module_opts_ketopt_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_opts_ketopt_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_opts_ketopt_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"opts_ketopt\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_opts_ketopt_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_opts_ketopt_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_opts_import_functions(st_modctx_t *opts_ctx,
 st_modctx_t *logger_ctx) {
    st_opts_ketopt_t *module = opts_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "st_logger_error");
    if (!module->logger.error) {
        fprintf(stderr,
         "opts_ketopt: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION(logger, debug);
    ST_LOAD_FUNCTION(logger, info);
    ST_LOAD_FUNCTION(logger, warning);

    return true;
}

static st_modctx_t *st_opts_init(int argc, char **argv,
 st_modctx_t *logger_ctx) {
    st_modctx_t      *opts_ctx;
    st_opts_ketopt_t *opts;
    errno_t           err;

    opts_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_opts_ketopt_data, sizeof(st_opts_ketopt_t));

    if (!opts_ctx)
        return NULL;

    opts_ctx->funcs = &st_opts_ketopt_funcs;

    opts = opts_ctx->data;
    opts->logger.ctx = logger_ctx;

    if (!st_opts_import_functions(opts_ctx, logger_ctx))
        return NULL;

    opts->argc = argc;
    opts->argv = argv;

    err = memset_s(opts->opts, sizeof(st_opt_t) * ST_OPTS_OPTS_MAX, '\0',
     sizeof(st_opt_t) * ST_OPTS_OPTS_MAX);
    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init opts_ketopt: %s\n", err_msg_buf);

        return NULL;
    }

    opts->opts_count = 0;

    opts->logger.info(opts->logger.ctx, "%s", "opts_ketopt: Opts initialized.");

    return opts_ctx;
}

static void st_opts_quit(st_modctx_t *opts_ctx) {
    st_opts_ketopt_t *opts = opts_ctx->data;

    opts->logger.info(opts->logger.ctx, "%s", "opts_ketopt: Destroying opts.");
    for (unsigned i = 0; i < opts->opts_count; i++) {
        if (opts->opts[i].longopt)
            free(opts->opts[i].longopt);
        if (opts->opts[i].arg_fmt)
            free(opts->opts[i].arg_fmt);
        if (opts->opts[i].opt_descr)
            free(opts->opts[i].opt_descr);
    }

    opts->logger.info(opts->logger.ctx, "%s", "opts_ketopt: Opts destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, opts_ctx);
}

static bool st_opts_add_option(st_modctx_t *opts_ctx, char short_option,
 const char *long_option, st_opt_arg_t arg, const char *arg_fmt,
 const char *option_descr) {
    st_opts_ketopt_t *opts = opts_ctx->data;
    st_opt_t         *opt = &opts->opts[opts->opts_count];
    bool              opt_set = false;

    if (!long_option || long_option[0] == '\0' || long_option[1] == '\0') {
        opts->logger.error(opts->logger.ctx,
         "opts_ketopt: Incorrect long option: %s", long_option);
    } else {
        opt->longopt = strdup(long_option);
        if (!opt->longopt) {
            opts->logger.error(opts->logger.ctx,
             "opts_ketopt: Unable to allocate memory for long option \"%s\": "
             "%s", long_option, strerror(errno));
        } else {
            opt->longopt_index = (int)opts->opts_count +
             LONG_OPT_NUM_TO_INDEX_OFFSET;
            opt_set = true;
        }
    }

    if (short_option) {
        if (isalnum(short_option)) {
            opt->shortopt = short_option;
            opt_set = true;
        } else {
            opts->logger.error(opts->logger.ctx,
             "opts_ketopt: Incorrect character for short option \"%c\"",
             short_option);
        }
    }

    if (!opt_set)
        return false;

    opt->arg = arg;

    if (arg_fmt) {
        opt->arg_fmt = strdup(arg_fmt);
        if (!opt->arg_fmt)
            opts->logger.warning(opts->logger.ctx,
             "opts_ketopt: Unable to allocate memory for option argument "
             "format");
    }
    if (option_descr) {
        opt->opt_descr = strdup(option_descr);
        if (!opt->opt_descr)
            opts->logger.warning(opts->logger.ctx,
             "opts_ketopt: Unable to allocate memory for option description");
    }

    opts->opts_count++;

    return true;
}

static bool st_opts_get_str(st_modctx_t *opts_ctx, const char *opt, char *dst,
 size_t dst_size_max) {
    st_opts_ketopt_t *opts = opts_ctx->data;
    char              short_opts_fmt[SHORT_OPTS_FMT_SIZE] = "";
    ko_longopt_t      longopts[LONGOPTS_COUNT] = {0};
    size_t            longopts_count = 0;
    int               longopt_index = -1;
    char              shortopt = '\0';
    ketopt_t          kopt = KETOPT_INIT;

    if (!dst || !dst_size_max)
        return false;

    if (!opt || opt[0] == '\0') {
        opts->logger.error(opts->logger.ctx, "opts_ketopt: Empty option");

        return false;
    }

    for (unsigned i = 0; i < opts->opts_count; i++) {
        if (opts->opts[i].shortopt) {
            errno_t err;
            char short_opt_fmt[SHORTOPT_FMT_SIZE] = {opts->opts[i].shortopt,
             '\0', '\0'};

            if (opts->opts[i].arg != ST_OA_NO)
                short_opt_fmt[1] = opts->opts[i].arg == ST_OA_REQUIRED
                 ? ':' : '?';

            err = strncat_s(short_opts_fmt, SHORT_OPTS_FMT_SIZE, short_opt_fmt,
             SHORTOPT_FMT_SIZE);
            if (err) {
                strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
                opts->logger.error(opts->logger.ctx,
                 "opts_ketopt: Unable to construct shortopts format: %s",
                 err_msg_buf);

                return false;
            }
        }

        if (opts->opts[i].longopt) {
            if (strcmp(opts->opts[i].longopt, opt) == 0 ||
             (opt[1] == '\0' && opts->opts[i].shortopt == opt[0])) {
                longopt_index = opts->opts[i].longopt_index;
                shortopt = opts->opts[i].shortopt;
            }

            longopts[longopts_count].name = opts->opts[i].longopt;
            longopts[longopts_count].has_arg = (int)opts->opts[i].arg;
            longopts[longopts_count].val = opts->opts[i].longopt_index;
            longopts_count++;
        }
    }

    while (true) {
        int parse_result = ketopt(&kopt, opts->argc, opts->argv, true,
         short_opts_fmt, longopts);

        if (parse_result == '?' || parse_result == ':')
            continue;

        if (parse_result < 0)
            break;

        if (parse_result == longopt_index ||
         (opt[1] == '\0' && parse_result == opt[0]) ||
         parse_result == shortopt) {
            if (!kopt.arg) {
                dst[0] = '\0';

                return true;
            }

            errno_t err = strcpy_s(dst, dst_size_max, kopt.arg);
            if (err) {
                strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
                opts->logger.error(opts->logger.ctx,
                 "opts_ketopt: Unable to get option argument: %s", err_msg_buf);

                return false;
            }

            return true;
        }
    }

    return false;
}
