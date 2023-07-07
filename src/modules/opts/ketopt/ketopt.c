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
#define SHORT_OPTS_FMT_SIZE          100
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

static void st_opts_import_functions(st_modctx_t *opts_ctx,
 st_modctx_t *logger_ctx) {
    st_opts_ketopt_t  *opts = opts_ctx->data;
    st_logger_funcs_t *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;

    opts->logger.debug = logger_funcs->logger_debug;
    opts->logger.info = logger_funcs->logger_info;
    opts->logger.error = logger_funcs->logger_error;
}

static st_modctx_t *st_opts_init(int argc, char **argv,
 st_modctx_t *logger_ctx) {
    st_modctx_t      *opts_ctx;
    st_opts_ketopt_t *opts;
    errno_t           err;

    opts_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_opts_ketopt_data, sizeof(st_opts_ketopt_t));

    if (opts_ctx == NULL)
        return NULL;

    opts_ctx->funcs = &st_opts_ketopt_funcs;

    st_opts_import_functions(opts_ctx, logger_ctx);
    opts = opts_ctx->data;
    opts->logger.ctx = logger_ctx;
    opts->argc = argc;
    opts->argv = argv;

    err = memset_s(opts->longopts, sizeof(ko_longopt_t) * ST_OPTS_OPTS_MAX,
     '\0', sizeof(ko_longopt_t) * ST_OPTS_OPTS_MAX);
    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init opts_ketopt: %s\n", err_msg_buf);

        return NULL;
    }

    opts->longopts_count = 0;
    opts->opts_data_count = 0;

    opts->logger.info(opts->logger.ctx, "%s", "opts_ketopt: Opts initialized.");

    return opts_ctx;
}

static void st_opts_quit(st_modctx_t *opts_ctx) {
    st_opts_ketopt_t *opts = opts_ctx->data;

    opts->logger.info(opts->logger.ctx, "%s", "opts_ketopt: Destroying opts.");
    for (unsigned i = 0; i < opts->longopts_count; i++)
        free(opts->longopts[i].name);

    for (unsigned i = 0; i < opts->opts_data_count; i++) {
        free(opts->opts_data[i].arg_fmt);
        free(opts->opts_data[i].opt_descr);
    }

    opts->logger.info(opts->logger.ctx, "%s", "opts_ketopt: Opts destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, opts_ctx);
}

static bool st_opts_add_long_option(st_opts_ketopt_t *opts,
 const char *long_option, st_opt_arg_t arg) {
    ko_longopt_t *ko_longopt = &opts->longopts[opts->longopts_count];
    errno_t       err;

    if (long_option == NULL || long_option[0] == '\0' || long_option[1] == '\0')
        return false;

    ko_longopt->name = strdup(long_option);
    if (!ko_longopt->name) {
        opts->logger.error(opts->logger.ctx,
         "opts_ketopt: Unable to add long option: %s", strerror(errno));

        return false;
    }

    ko_longopt->has_arg = (int)arg;
    ko_longopt->val = (int)opts->longopts_count + LONG_OPT_NUM_TO_INDEX_OFFSET;

    return true;
}

static bool st_opts_add_option(st_modctx_t *opts_ctx, char short_option,
 const char *long_option, st_opt_arg_t arg, const char *arg_fmt,
 const char *option_descr) {
    st_opts_ketopt_t *opts = opts_ctx->data;
    st_opt_data_t    *opt_data = &opts->opts_data[opts->longopts_count];
    bool              longopt_add_success = false;

    if (long_option)
        longopt_add_success = st_opts_add_long_option(opts, long_option, arg);

    if (isgraph(short_option)) {
        opt_data->short_opt = short_option;
        opt_data->arg = arg;
        if (longopt_add_success)
            opt_data->longopt_index = (int)opts->longopts_count +
             LONG_OPT_NUM_TO_INDEX_OFFSET;
        else
            opt_data->longopt_index = ST_OPTS_LONG_INDEX_UNSPEC;
    } else {
        if (!longopt_add_success)
            return false;
    }
    if (arg_fmt) {
        opt_data->arg_fmt = strdup(arg_fmt);
        if (!opt_data->arg_fmt) {
            opts->logger.error(opts->logger.ctx,
             "opts_ketopt: Unable to allocate memory for option argument "
             "format");

            return false;
        }
    }
    if (option_descr) {
        opt_data->opt_descr = strdup(option_descr);
        if (!opt_data->opt_descr) {
            opts->logger.error(opts->logger.ctx,
             "opts_ketopt: Unable to allocate memory for option description");
            free(opt_data->arg_fmt);

            return false;
        }
    }
    if (longopt_add_success)
        opts->longopts_count++;

    opts->opts_data_count++;

    return true;
}

static int st_get_index_by_longopt(st_opts_ketopt_t *opts,
 const char *longopt) {
    for (unsigned i = 0; i < opts->longopts_count; i++) {
        if (strcmp(opts->longopts->name, longopt) == 0)
            return (int)i;
    }

    return -1;
}

static bool st_opts_get_longopt_str(const st_opts_ketopt_t *opts,
 int kopt_parse_result, const ketopt_t *kopt, unsigned req_longopt_index,
 char *dst, size_t dst_size) {
    for (unsigned i = 0; i < opts->opts_data_count; i++) {
        if (kopt_parse_result == opts->opts_data[i].longopt_index ||
         kopt_parse_result == opts->opts_data[i].short_opt) {
            if (i != req_longopt_index)
                return false;

            if (!kopt->arg) {
                if (strncpy_s(dst, dst_size, "", 1) != 0)
                    return false;
            } else if (strcpy_s(dst, dst_size, kopt->arg) != 0) {
                return false;
            }

            return true;
        }
    }

    return false;
}

static bool st_opts_get_str(st_modctx_t *opts_ctx, const char *opt,
 char *optarg, size_t optarg_size_max) {
    st_opts_ketopt_t *opts = opts_ctx->data;
    char              short_opts_fmt[SHORT_OPTS_FMT_SIZE] = "";
    st_opttype_t      opt_type_required;
    ketopt_t          kopt = KETOPT_INIT;

    if (!opt || opt[0] == '\0') {
        opts->logger.error(opts->logger.ctx, "opts_ketopt: Empty option");

        return false;
    }

    opt_type_required = opt[1] == '\0' ? ST_OT_SHORT : ST_OT_LONG;

    for (unsigned i = 0; i < opts->opts_data_count; i++) {
        if (opts->opts_data[i].short_opt != ST_OPTS_SHORT_UNSPEC) {
            errno_t err;
            char short_opt_fmt[3] = {opts->opts_data[i].short_opt, '\0', '\0'};

            if (opts->opts_data[i].arg != ST_OA_NO)
                short_opt_fmt[1] = opts->opts_data[i].arg == ST_OA_REQUIRED ?
                 ':' : '?';

            err = strncat_s(short_opts_fmt, SHORT_OPTS_FMT_SIZE, short_opt_fmt,
             3);
            if (err) {
                strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
                opts->logger.error(opts->logger.ctx,
                 "opts_ketopt: Unable to construct shortopts format: %s",
                 err_msg_buf);

                return false;
            }
        }
    }

    kopt = KETOPT_INIT;
    while (true) {
        int parse_result = ketopt(&kopt, opts->argc, opts->argv, 1,
         short_opts_fmt, opts->longopts);

        if (parse_result < 0 || parse_result == '?' || parse_result == ':')
            break;

        if (opt_type_required == ST_OT_LONG) {
            int  req_longopt_index = st_get_index_by_longopt(opts, opt);
            bool opt_found;

            if (req_longopt_index == -1)
                continue;

            opt_found = st_opts_get_longopt_str(opts, parse_result, &kopt,
             (unsigned)req_longopt_index, optarg, optarg_size_max);

            if (opt_found)
                return true;
        } else {
            if (parse_result == opt[0]) {
                if (!kopt.arg) {
                    if (optarg_size_max > 0)
                        optarg[0] = '\0';
                } else {
                    errno_t err = strcpy_s(optarg, optarg_size_max, kopt.arg);

                    if (err) {
                        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
                        opts->logger.error(opts->logger.ctx,
                         "opts_ketopt: Unable to get option argument: %s",
                         err_msg_buf);

                        return false;
                    }
                }

                return true;
            }
        }
    }

    opts->logger.error(opts->logger.ctx, "opts_ketopt: Missing option: %s",
     opt);

    return false;
}
