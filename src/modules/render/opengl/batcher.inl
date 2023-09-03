#ifndef ST_MODULES_RENDER_OPENGL_BATCHER_INL
#define ST_MODULES_RENDER_OPENGL_BATCHER_INL

#define INITIAL_ENTRIES_CAPACITY 8192
#define VERTICES_PER_TEXTURE     6

static bool batcher_init(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;
    st_batcher_t       *batcher = &module->batcher;

    batcher->entries = module->dynarr.create(module->dynarr.ctx,
     sizeof(st_batch_entry_t), INITIAL_ENTRIES_CAPACITY);

    if (!batcher->entries) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to create dynamic array for batcher entries");

        return false;
    }

    batcher->module = module;
    batcher->current_texture = NULL;
    batcher->current_first_vertex_index = 0;
    batcher->current_vertex_index = 0;

    return true;
}

static void batcher_free(st_batcher_t *batcher) {
    batcher->module->dynarr.destroy(batcher->entries);
}

bool batcher_clear(st_batcher_t *batcher) {
    batcher->current_texture = NULL;
    batcher->current_first_vertex_index = 0;
    batcher->current_vertex_index = 0;
    return batcher->module->dynarr.clear(batcher->entries);
}

static void batcher_finalize(st_batcher_t *batcher) {
    batcher->module->dynarr.append(batcher->entries, &(st_batch_entry_t){
        batcher->current_texture,
        batcher->current_first_vertex_index,
        batcher->current_vertex_index - batcher->current_first_vertex_index,
    });
}

static void batcher_process_texture(st_batcher_t *batcher,
 const st_texture_t *texture) {
    if (!batcher->current_texture)
        batcher->current_texture = texture;

    if (texture != batcher->current_texture) {
        batcher_finalize(batcher);
        batcher->current_first_vertex_index = batcher->current_vertex_index;
        batcher->current_texture = texture;
    }

    batcher->current_vertex_index += VERTICES_PER_TEXTURE;
}

static bool batcher_bind_texture(st_batcher_t *batcher, size_t entry_index) {
    const st_batch_entry_t *entry = batcher->module->dynarr.get(
     batcher->entries, entry_index);

    return batcher->module->texture.bind(entry->texture);
}

static GLsizei batcher_get_first_vertex_index(const st_batcher_t *batcher,
 size_t entry_index) {
    const st_batch_entry_t *entry = batcher->module->dynarr.get(
     batcher->entries, entry_index);

    return (GLsizei)entry->first_vertex_index;
}

static GLint batcher_get_vertices_count(const st_batcher_t *batcher,
 size_t entry_index) {
    const st_batch_entry_t *entry = batcher->module->dynarr.get(
     batcher->entries, entry_index);

    return (GLint)entry->vertices_count;
}

static size_t batcher_get_entries_count(const st_batcher_t *batcher) {
    return batcher->module->dynarr.get_elements_count(batcher->entries);
}

#endif /* ST_MODULES_RENDER_OPENGL_BATCHER_INL */
