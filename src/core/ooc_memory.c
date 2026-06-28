#include "ooc_memory.h"
#include <stdlib.h>
#include <threads.h>

#define POOL_MAX_BUCKETS 128
static void *freelists[POOL_MAX_BUCKETS];
static mtx_t pool_mutex;

__attribute__((constructor))
static void pool_init(void) {
    mtx_init(&pool_mutex, mtx_plain);
}

__attribute__((destructor))
static void pool_fini(void) {
    pool_cleanup();
    mtx_destroy(&pool_mutex);
}

void *pool_alloc(size_t size) {
    size_t aligned = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    int idx = aligned / sizeof(void*);

    if (idx < POOL_MAX_BUCKETS) {
        mtx_lock(&pool_mutex);
        void *obj = freelists[idx];
        if (obj) {
            freelists[idx] = *(void**)obj;
            mtx_unlock(&pool_mutex);
            return obj;
        }
        mtx_unlock(&pool_mutex);
    }

    size_t *header = malloc(sizeof(size_t) + aligned);
    if (!header) return NULL;
    *header = aligned;
    return header + 1;
}

void pool_free(void *ptr) {
    if (!ptr) return;
    size_t *header = (size_t *)ptr - 1;
    size_t aligned = *header;
    int idx = aligned / sizeof(void*);

    if (idx < POOL_MAX_BUCKETS) {
        mtx_lock(&pool_mutex);
        *(void **)ptr = freelists[idx];
        freelists[idx] = ptr;
        mtx_unlock(&pool_mutex);
    } else {
        free(header);
    }
}
void pool_cleanup(void) {
    mtx_lock(&pool_mutex);
    for (int i = 0; i < POOL_MAX_BUCKETS; i++) {
        void *ptr = freelists[i];
        while (ptr) {
            void *next = *(void**)ptr;
            size_t *header = (size_t *)ptr - 1;
            free(header);
            ptr = next;
        }
        freelists[i] = NULL;
    }
    mtx_unlock(&pool_mutex);
}
