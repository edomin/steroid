## atlas
```luau
Atlas = require "Atlas"

Atlas.new_ctx(logger_ctx: logger_ctx, texture_ctx: texture_ctx): atlas_ctx

atlas_ctx:destroy()
```

## bitmap
```luau
Bitmap = require "Bitmap"

Bitmap.new_ctx(logger_ctx: logger_ctx): bitmap_ctx

bitmap_ctx:destroy()
```

## drawq
```luau
Drawq = require "Drawq"

Drawq.new_ctx(dynarr_ctx: dynarr_ctx, logger_ctx: logger_ctx, sprite_ctx: sprite_ctx): drawq_ctx

drawq_ctx:destroy()
```

## dynarr
```luau
Dynarr = require "Dynarr"

Dynarr.new_ctx(logger_ctx: logger_ctx): dynarr_ctx

dynarr_ctx:destroy()
```

## events
```luau
Events = require "Events"

Events.new_ctx(logger_ctx: logger_ctx): events_ctx

events_ctx:destroy()
```

## fs
```luau
Fs = require "Fs"

Fs.new_ctx(logger_ctx: logger_ctx, pathtools_ctx: pathtools_ctx): fs_ctx
Fs.ft_unknown: integer
Fs.ft_reg: integer
Fs.ft_dir: integer
Fs.ft_chr: integer
Fs.ft_blk: integer
Fs.ft_fifo: integer
Fs.ft_link: integer
Fs.ft_sock: integer

fs_ctx:destroy()
fs_ctx:get_file_type(filename: string): integer
fs_ctx:mkdir(dirname: string): bool
```

## gfxctx
```luau
GfxCtx = require "GfxCtx"

GfxCtx.new_ctx(logger_ctx: logger_ctx, monitor_ctx: monitor_ctx, window_ctx: window_ctx): gfxctx_ctx
GfxCtx.gapi_gl11: integer
GfxCtx.gapi_gl12: integer
GfxCtx.gapi_gl13: integer
GfxCtx.gapi_gl14: integer
GfxCtx.gapi_gl15: integer
GfxCtx.gapi_gl2: integer
GfxCtx.gapi_gl21: integer
GfxCtx.gapi_gl3: integer
GfxCtx.gapi_gl31: integer
GfxCtx.gapi_gl32: integer
GfxCtx.gapi_gl33: integer
GfxCtx.gapi_gl4: integer
GfxCtx.gapi_gl41: integer
GfxCtx.gapi_gl42: integer
GfxCtx.gapi_gl43: integer
GfxCtx.gapi_gl44: integer
GfxCtx.gapi_gl45: integer
GfxCtx.gapi_gl46: integer
GfxCtx.gapi_es1: integer
GfxCtx.gapi_es11: integer
GfxCtx.gapi_es2: integer
GfxCtx.gapi_es3: integer
GfxCtx.gapi_es31: integer
GfxCtx.gapi_es32: integer

gfxctx_ctx:destroy()
gfxctx_ctx:create(monitor: monitor, window: window, api: integer): gfxctx
gfxctx_ctx:create_shared(monitor: monitor, window: window, gfxctx: gfxctx): gfxctx

gfxctx:make_current(): bool
gfxctx:swap_buffers(): bool
gfxctx:get_api(): integer
gfxctx:destroy()
```

## logger
```luau
Logger = require "Logger"

Logger.new_ctx(): logger_ctx
Logger.get_ctx(): logger_ctx
Logger.ll_none: integer
Logger.ll_error: integer
Logger.ll_warning: integer
Logger.ll_info: integer
Logger.ll_debug: integer
Logger.ll_all: integer

logger_ctx:destroy()
logger_ctx:set_stdout_levels(levels: integer): bool
logger_ctx:set_stderr_levels(levels: integer): bool
logger_ctx:set_syslog_levels(levels: integer): bool
logger_ctx:set_log_file(filename: string, levels: integer): bool
logger_ctx:set_callback(): bool
logger_ctx:debug(msg: string)
logger_ctx:info(msg: string)
logger_ctx:warning(msg: string)
logger_ctx:error(msg: string)
logger_ctx:set_postmortem_msg(msg: string)
```

## monitor
```luau
Monitor = require "Monitor"

Monitor.new_ctx(logger_ctx: logger_ctx): monitor_ctx

monitor_ctx:destroy()
monitor_ctx:get_monitors_count(): integer
monitor_ctx:open(index: integer): monitor

monitor:release()
monitor:get_width(): integer
monitor:get_height(): integer
```

## opts
```luau
Opts = require "Opts"

Opts.new_ctx(logger_ctx: logger_ctx): nil
Opts.get_ctx(): opts_ctx
Opts.oa_no: integer
Opts.oa_required: integer
Opts.oa_optional: integer

opts_ctx:destroy()
opts_ctx:add_option(shortopt: string, longopt: ?string, argreq: integer, argfmt: ?string, optdescr: ?string): bool
opts_ctx:get_str(opt: string): ?string
opts_ctx:get_help(columns: integer): ?string
```

## pathtools
```luau
PathTools = require "PathTools"

PathTools.new_ctx(logger_ctx: logger_ctx): pathtools_ctx

pathtools_ctx:destroy()
pathtools_ctx:get_parent_dir(path: string): ?string
pathtools_ctx:concat(path: string, append: string): ?string
```

## png
```luau
Png = require "Png"

Png.new_ctx(logger_ctx: logger_ctx): png_ctx

png_ctx:destroy()
```

## sprite
```luau
Sprite = require "Sprite"

Sprite.new_ctx(atlas_ctx: atlas_ctx, logger_ctx: logger_ctx, texture_ctx: texture_ctx): sprite_ctx

sprite_ctx:destroy()
sprite_ctx:from_texture(texture: texture): sprite
sprite:destroy()
sprite:get_width(): integer
sprite:get_height(): integer
```

## timer
```luau
Timer = require "Timer"

Timer.new_ctx(logger_ctx: logger_ctx): timer_ctx

timer_ctx:destroy()
timer_ctx:start(): integer
timer_ctx:get_elapsed(start: integer): integer
timer_ctx:sleep(ms: integer)
timer_ctx:sleep_for_fps(fps: integer)
```

## terminal
```luau
Terminal = require "Terminal"

Terminal.new_ctx(logger_ctx: logger_ctx): terminal_ctx

terminal_ctx:destroy()
terminal_ctx:get_rows_count(): integer
terminal_ctx:get_cols_count(): integer
```

## texture
```luau
Texture = require "Texture"

Texture.new_ctx(bitmap_ctx: bitmap_ctx, logger_ctx: logger_ctx, api: int): texture_ctx

texture_ctx:destroy()
texture_ctx:load(filename: string): texture
texture:destroy()
texture:bind(): bool
texture:get_width(): integer
texture:get_height(): integer
```

## window
```luau
Window = require "Window"

Window.new_ctx(events_ctx: events_ctx, logger_ctx: logger_ctx, monitor_ctx: monitor_ctx): window_ctx

window_ctx:destroy()
window_ctx:create(monitor_ctx: monitor_ctx, x: integer, y: integer, width: integer, height: integer, fullscreen: bool, title: string): window
window_ctx:process()
window:destroy()
window:xed(): bool
window:get_width(): integer
window:get_height(): integer
```
