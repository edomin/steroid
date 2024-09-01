#pragma once

#define ST_CLASS(...)    \
typedef struct {         \
    st_object_t _object; \
    __VA_ARGS__;         \
}

#define ST_STRUCT_CLASS(struct_name, ...) \
typedef struct struct_name {              \
    st_object_t _object;                  \
    __VA_ARGS__;                          \
}

typedef struct {
    void *owner;
    void *funcs;
} st_object_t;

static inline void st_object_make(void *object, void *owner, void *funcs) {
    ((st_object_t *)object)->owner = owner;
    ((st_object_t *)object)->funcs = funcs;
}

static inline void *st_object_get_owner(const void *object) {
    return ((const st_object_t *)object)->owner;
}

static inline void *st_object_get_funcs(void *object) {
    return ((st_object_t *)object)->funcs;
}
