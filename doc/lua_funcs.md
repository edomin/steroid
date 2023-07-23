## fs
```luau
st_fs(logger: st_logger, pathtools: st_pathtools): st_fs
st_fs:quit()
st_fs:get_file_type(filename: string): integer
st_fs:mkdir(dirname: string): bool
st_fs.ft_unknown: integer
st_fs.ft_reg: integer
st_fs.ft_dir: integer
st_fs.ft_chr: integer
st_fs.ft_blk: integer
st_fs.ft_fifo: integer
st_fs.ft_link: integer
st_fs.ft_sock: integer
```

## logger
```luau
st_logger(): st_logger
st_logger_get_instance(): st_logger
st_logger:quit()
st_logger:set_stdout_levels(levels: integer): bool
st_logger:set_stderr_levels(levels: integer): bool
st_logger:set_syslog_levels(levels: integer): bool
st_logger:set_log_file(filename: string, levels: integer): bool
st_logger:set_callback(): bool
st_logger:debug(msg: string)
st_logger:info(msg: string)
st_logger:warning(msg: string)
st_logger:error(msg: string)
st_logger.ll_none: integer
st_logger.ll_error: integer
st_logger.ll_warning: integer
st_logger.ll_info: integer
st_logger.ll_debug: integer
st_logger.ll_all: integer
```

## opts
```luau
st_opts_init(): nil
st_opts_get_instance(): st_opts
st_opts:quit()
st_opts:add_option(shortopt: string, longopt: ?string, argreq: integer, argfmt: ?string, optdescr: ?string): bool
st_opts:get_str(opt: string): ?string
st_opts:get_help(columns: integer): ?string
st_opts.oa_no: integer
st_opts.oa_required: integer
st_opts.oa_optional: integer
```

## pathtools
```luau
st_pathtools(): st_pathtools
st_pathtools:quit()
st_pathtools:get_parent_dir(path: string): ?string
st_pathtools:concat(path: string, append: string): ?string
```
