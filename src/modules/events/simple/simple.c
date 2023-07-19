#include "simple.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_events_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_events_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_events_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"events_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_events_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_events_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_events_import_functions(st_modctx_t *events_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *rbuf_ctx) {
    st_events_simple_t *module = events_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "events_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION("rbuf_simple", logger, debug);
    ST_LOAD_FUNCTION("rbuf_simple", logger, info);

    ST_LOAD_FUNCTION("rbuf_simple", rbuf, create);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, destroy);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, push);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, peek);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, pop);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, drop);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, clear);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, get_free_space);
    ST_LOAD_FUNCTION("rbuf_simple", rbuf, is_empty);

    return true;
}

static st_modctx_t *st_events_init(st_modctx_t *logger_ctx,
 st_modctx_t *rbuf_ctx) {
    st_modctx_t        *events_ctx;
    st_events_simple_t *events;

    events_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_events_simple_data, sizeof(st_events_simple_t));

    if (events_ctx == NULL)
        return NULL;

    events_ctx->funcs = &st_events_simple_funcs;

    events = events_ctx->data;
    events->logger.ctx = logger_ctx;
    events->rbuf.ctx = rbuf_ctx;

    if (!st_events_import_functions(events_ctx, logger_ctx, rbuf_ctx))
        return NULL;

    events->types_count = 0;

    events->logger.info(events->logger.ctx,
     "events_simple: Event subsystem initialized.");

    return events_ctx;
}

static void st_events_quit(st_modctx_t *events_ctx) {
    st_events_simple_t *module = events_ctx->data;

    // TODO(edomin):
    // Unsubscribe all queues
    // Delete queues

    module->logger.info(module->logger.ctx,
     "events_simple: Event subsystem destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, events_ctx);
}

static st_evtypeid_t st_events_register_type(st_modctx_t *events_ctx,
 const char *type_name, size_t size) {
    st_events_simple_t *module = events_ctx->data;
    st_evtype_t        *evtype = &module->types[module->types_count];
    errno_t             err = strcpy_s(evtype->name, EVENT_TYPE_NAME_SIZE,
     type_name);

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        module->logger.error(module->logger.ctx,
         "events_simple: Unable to copy event type name while registering "
         "event type \"%s\": %s", type_name, err_msg_buf);

        return ST_EVTYPE_ID_NONE;
    }

    err = memset_s(evtype->subscribers, sizeof(st_evq_t *) * SUBSCRIBERS_MAX, 0,
     sizeof(st_evq_t *) * SUBSCRIBERS_MAX);
    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        module->logger.error(module->logger.ctx,
         "events_simple: Unable to reset subscribers while registering event "
         "type \"%s\": %s", type_name, err_msg_buf);

        return ST_EVTYPE_ID_NONE;
    }

    evtype->data_size = size;
    evtype->subscribers_count = 0;

    return (st_evtypeid_t)(module->types_count++);
}

static st_evtypeid_t st_events_get_type_id(st_modctx_t *events_ctx,
 const char *type_name) {
    st_events_simple_t *module = events_ctx->data;

    for (size_t i = 0; i < module->types_count; i++) {
        if (strcmp(module->types[i].name, type_name) == 0)
            return (st_evtypeid_t)i;
    }

    return ST_EVTYPE_ID_NONE;
}

static st_evq_t *st_events_create_queue(st_modctx_t *events_ctx,
 size_t pool_size) {
    st_events_simple_t *module = events_ctx->data;
    st_rbuf_t *handle = module->rbuf.create(module->rbuf.ctx, pool_size);
    st_evq_t  *queue;

    if (!handle)
        return NULL;

    queue = malloc(sizeof(st_evq_t));
    if (!queue) {
        module->rbuf.destroy(handle);

        return NULL;
    }

    queue->ctx = events_ctx;
    queue->handle = handle;
    queue->active = true;

    return queue;
}

static void st_events_destroy_queue(st_evq_t *queue) {
    st_events_simple_t *module = queue->ctx->data;

    st_events_unsubscribe_all(queue);

    module->rbuf.destroy(queue->handle);
    free(queue);
}

static bool st_events_subscribe(st_evq_t *queue, st_evtypeid_t type_id) {
    st_events_simple_t *module = queue->ctx->data;
    st_evtype_t        *evtype;

    if (type_id >= (st_evtypeid_t)module->types_count)
        return false;

    evtype = &module->types[type_id];

    for (size_t i = 0; i < evtype->subscribers_count; i++) {
        if (evtype->subscribers[i] == queue)
            return true;
    }

    if (evtype->subscribers_count >= SUBSCRIBERS_MAX)
        return false;

    evtype->subscribers[evtype->subscribers_count++] = queue;

    return true;
}

static void st_events_unsubscribe(st_evq_t *queue, st_evtypeid_t type_id) {
    st_events_simple_t *module = queue->ctx->data;
    st_evtype_t        *evtype;

    if (type_id >= (st_evtypeid_t)module->types_count)
        return;

    evtype = &module->types[type_id];

    for (size_t i = 0; i < evtype->subscribers_count; i++) {
        if (evtype->subscribers[i] == queue) {
            size_t block_size = sizeof(st_evq_t *) *
             (evtype->subscribers_count - i - 1);

            if (i <= evtype->subscribers_count - 1 &&
             memmove_s(&evtype->subscribers[i], block_size,
              &evtype->subscribers[i + 1], block_size) == 0)
                evtype->subscribers_count--;

            return;
        }
    }
}

static void st_events_unsubscribe_all(st_evq_t *queue) {
    st_events_simple_t *module = queue->ctx->data;

    for (int i = 0; i < (st_evtypeid_t)module->types_count; i++)
        st_events_unsubscribe(queue, i);
}

static void st_events_suspend(st_evq_t *queue, bool clear) {
    if (clear)
        st_events_clear(queue);

    queue->active = false;
}

static void st_events_resume(st_evq_t *queue) {
    queue->active = true;
}

static void st_events_push(st_modctx_t *events_ctx, st_evtypeid_t type_id,
 const void *data) {
    st_events_simple_t *module = events_ctx->data;
    st_evtype_t        *evtype;

    if (type_id >= (st_evtypeid_t)module->types_count)
        return;

    evtype = &module->types[type_id];

    for (size_t i = 0; i < evtype->subscribers_count; i++) {
        if (!evtype->subscribers[i]->active)
            continue;

        if (module->rbuf.get_free_space(evtype->subscribers[i]->handle) >=
         evtype->data_size + sizeof(st_evtypeid_t)) {
            module->rbuf.push(evtype->subscribers[i]->handle, &type_id,
             sizeof(st_evtypeid_t));
            module->rbuf.push(evtype->subscribers[i]->handle, data,
             evtype->data_size);
        }
    }
}

static bool st_events_is_empty(const st_evq_t *queue) {
    st_events_simple_t *module = queue->ctx->data;

    return module->rbuf.is_empty(queue->handle);
}

static st_evtypeid_t st_events_peek_type(const st_evq_t *queue) {
    st_events_simple_t *module = queue->ctx->data;
    st_evtypeid_t       type_id;
    bool                success = module->rbuf.peek(queue->handle, &type_id,
     sizeof(st_evtypeid_t));

    return success ? type_id : ST_EVTYPE_ID_NONE;
}

static bool st_events_pop(st_evq_t *queue, void *data) {
    st_events_simple_t *module = queue->ctx->data;
    st_evtypeid_t       type_id;

    if (!module->rbuf.pop(queue->handle, &type_id, sizeof(st_evtypeid_t)))
        return false;

    return module->rbuf.pop(queue->handle, data,
     module->types[type_id].data_size);
}

static bool st_events_drop(st_evq_t *queue) {
    st_events_simple_t *module = queue->ctx->data;
    st_evtypeid_t       type_id;

    if (!module->rbuf.pop(queue->handle, &type_id, sizeof(st_evtypeid_t)))
        return false;

    return module->rbuf.drop(queue->handle, module->types[type_id].data_size);
}

static bool st_events_clear(st_evq_t *queue) {
    st_events_simple_t *module = queue->ctx->data;

    return module->rbuf.clear(queue->handle);
}
