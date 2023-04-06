#include "ketopt.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <bsd/string.h>

#define LONG_OPT_NUM_TO_INDEX_OFFSET 300
#define SHORT_OPTS_FMT_SIZE          100

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

typedef enum {
    ST_OT_SHORT,
    ST_OT_LONG,
} st_opttype_t;

void *st_module_opts_ketopt_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_opts_ketopt_funcs_table;

    for (size_t i = 0; i < funcs_table->funcs_count; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_opts_ketopt_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    global_modsmgr = modsmgr;
    memcpy(&global_modsmgr_funcs, modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

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

    opts_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_opts_ketopt_data, sizeof(st_opts_ketopt_t));

    if (opts_ctx == NULL)
        return NULL;

    opts_ctx->funcs = &st_opts_ketopt_funcs;

    st_opts_import_functions(opts_ctx, logger_ctx);
    opts = opts_ctx->data;
    // opts->logger.active = true;
    opts->logger.ctx = logger_ctx;
    opts->argc = argc;
    opts->argv = argv;
    memset(opts->longopts, 0, sizeof(ko_longopt_t) * ST_OPTS_OPTS_MAX);
    memset(opts->opts_data, 0, sizeof(st_opt_data_t) * ST_OPTS_OPTS_MAX);
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
    size_t        longopt_size = strlen(long_option) + 1;
    ko_longopt_t *ko_longopt = &opts->longopts[opts->longopts_count];

    if (longopt_size <= 2)
        return false;

    ko_longopt->name = malloc(sizeof(char) * longopt_size);
    if (ko_longopt->name == NULL) {
        opts->logger.error(opts->logger.ctx, "%s",
         "opts_ketopt: Unable to allocate memory for long option. Using "
         "short option only");
        long_option = NULL;

        return false;
    }

    strlcpy(ko_longopt->name, long_option, longopt_size);
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
    if (arg_fmt != NULL) {
        size_t arg_fmt_size = strlen(arg_fmt) + 1;

        opt_data->arg_fmt = malloc(sizeof(char) * arg_fmt_size);
        if (opt_data->arg_fmt == NULL)
            opts->logger.error(opts->logger.ctx, "%s",
             "opts_ketopt: Unable to allocate memory for option argument "
             "format");
        else
            strlcpy(opt_data->arg_fmt, arg_fmt, arg_fmt_size);
    }
    if (option_descr != NULL) {
        size_t option_descr_size = strlen(option_descr) + 1;

        opt_data->opt_descr = malloc(sizeof(char) * option_descr_size);
        if (opt_data->opt_descr == NULL)
            opts->logger.error(opts->logger.ctx, "%s",
             "opts_ketopt: Unable to allocate memory for option description");
        else
            strlcpy(opt_data->opt_descr, option_descr, option_descr_size);
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
        if (kopt_parse_result == opts->opts_data[i].longopt_index) {
            if (i != req_longopt_index)
                return false;

            if (!kopt->arg)
                strlcpy(dst, "", dst_size);
            else
                strlcpy(dst, kopt->arg, dst_size);

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
    size_t            optlen;
    ketopt_t          kopt = KETOPT_INIT;

    if (!opt)
        return false;

    optlen = strlen(opt);

    if (optlen == 0)
        return false;

    opt_type_required = optlen == 1 ? ST_OT_SHORT : ST_OT_LONG;

    for (unsigned i = 0; i < opts->opts_data_count; i++) {
        if (opts->opts_data[i].short_opt != ST_OPTS_SHORT_UNSPEC) {
            char short_opt_fmt[3] = {opts->opts_data[i].short_opt, '\0', '\0'};

            if (opts->opts_data[i].arg != ST_OA_NO)
                short_opt_fmt[1] = opts->opts_data[i].arg == ST_OA_REQUIRED ?
                 ':' : '?';

            strlcat(short_opts_fmt, short_opt_fmt, SHORT_OPTS_FMT_SIZE);
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
                if (!kopt.arg)
                    strlcpy(optarg, "", optarg_size_max);
                else
                    strlcpy(optarg, kopt.arg, optarg_size_max);

                return true;
            }
        }
    }

    return false;
}
