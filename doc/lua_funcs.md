## events
```luau
st_events(logger: st_logger): st_events
st_events:quit()
```

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

## gfxctx
```luau
st_gfxctx_ctx(logger: st_logger, monitor_ctx: st_monitor_ctx, window_ctx: st_window_ctx): st_gfxctx_ctx
st_gfxctx_ctx:quit()
st_gfxctx_ctx:create(monitor: st_monitor, window: st_window, api: integer): st_gfxctx
st_gfxctx_ctx:create_shared(monitor: st_monitor, window: st_window, gfxctx: st_gfxctx): st_gfxctx
st_gfxctx:make_current(): bool
st_gfxctx:swap_buffers(): bool
st_gfxctx:destroy()
st_gfxctx_ctx.gapi_gl11: integer
st_gfxctx_ctx.gapi_gl12: integer
st_gfxctx_ctx.gapi_gl13: integer
st_gfxctx_ctx.gapi_gl14: integer
st_gfxctx_ctx.gapi_gl15: integer
st_gfxctx_ctx.gapi_gl2: integer
st_gfxctx_ctx.gapi_gl21: integer
st_gfxctx_ctx.gapi_gl3: integer
st_gfxctx_ctx.gapi_gl31: integer
st_gfxctx_ctx.gapi_gl32: integer
st_gfxctx_ctx.gapi_gl33: integer
st_gfxctx_ctx.gapi_gl4: integer
st_gfxctx_ctx.gapi_gl41: integer
st_gfxctx_ctx.gapi_gl42: integer
st_gfxctx_ctx.gapi_gl43: integer
st_gfxctx_ctx.gapi_gl44: integer
st_gfxctx_ctx.gapi_gl45: integer
st_gfxctx_ctx.gapi_gl46: integer
st_gfxctx_ctx.gapi_es1: integer
st_gfxctx_ctx.gapi_es11: integer
st_gfxctx_ctx.gapi_es2: integer
st_gfxctx_ctx.gapi_es3: integer
st_gfxctx_ctx.gapi_es31: integer
st_gfxctx_ctx.gapi_es32: integer
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
st_logger:set_postmortem_msg(msg: string)
st_logger.ll_none: integer
st_logger.ll_error: integer
st_logger.ll_warning: integer
st_logger.ll_info: integer
st_logger.ll_debug: integer
st_logger.ll_all: integer
```

## monitor
```luau
st_monitor_ctx(logger: st_logger): st_monitor_ctx
st_monitor_ctx:quit()
st_monitor_ctx:get_monitors_count(): integer
st_monitor_ctx:open(index: integer): st_monitor
st_monitor:release()
st_monitor:get_width(): integer
st_monitor:get_height(): integer
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
st_pathtools(logger: st_logger): st_pathtools
st_pathtools:quit()
st_pathtools:get_parent_dir(path: string): ?string
st_pathtools:concat(path: string, append: string): ?string
```

## timer
```luau
st_timer(logger: st_logger): st_timer
st_timer:quit()
st_timer:start(): integer
st_timer:get_elapsed(start: integer): integer
st_timer:sleep(ms: integer)
st_timer:sleep_for_fps(fps: integer)
```

## terminal
```luau
st_terminal(logger: st_logger): st_terminal
st_terminal:quit()
st_terminal:get_rows_count(): integer
st_terminal:get_cols_count(): integer
```

## window
```luau
st_window_ctx(events: st_events, logger: st_logger, monitor_ctx: st_monitor_ctx): st_window_ctx
st_window_ctx:quit()
st_window_ctx:create(monitor: st_monitor, x: integer, y: integer, width: integer, height: integer, fullscreen: bool, title: string): st_window
st_window:destroy()
st_window_ctx:process()
```
