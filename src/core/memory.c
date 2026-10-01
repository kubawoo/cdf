#include "memory.h"
#include <stdlib.h>
#include <stdint.h>
#include <threads.h>

#define POOL_MAX_BUCKETS 128

/* The freelist next-pointer is stored in the first word of the user block, so
   the only word the pool owns exclusively is the size header in front of it.
   The top bit of that header marks the block as currently parked on a freelist,
   which lets pool_free recognise a second free of the same pointer instead of
   linking it twice and corrupting the list. */
#define POOL_PARKED   (SIZE_MAX & ~(SIZE_MAX >> 1))
#define POOL_SIZE_MAX (POOL_PARKED - 1)

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
    if (size > POOL_SIZE_MAX) return NULL;

    size_t aligned = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    /* The freelist link lives in the first word of the user block, so a pooled
       block must be at least one word wide even for a zero-sized request. */
    if (aligned == 0) aligned = sizeof(void*);
    int idx = aligned / sizeof(void*);

    if (idx < POOL_MAX_BUCKETS) {
        mtx_lock(&pool_mutex);
        void *obj = freelists[idx];
        if (obj) {
            freelists[idx] = *(void**)obj;
            *(size_t *)((size_t *)obj - 1) = aligned;   /* clear the parked mark */
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
    int idx;

    mtx_lock(&pool_mutex);
    size_t raw = *header;

    if (raw & POOL_PARKED) {                 /* already on a freelist: ignore */
        mtx_unlock(&pool_mutex);
        return;
    }

    size_t aligned = raw;
    idx = aligned / sizeof(void*);

    if (idx < POOL_MAX_BUCKETS) {
        *header = aligned | POOL_PARKED;
        *(void **)ptr = freelists[idx];
        freelists[idx] = ptr;
    } else {
        mtx_unlock(&pool_mutex);
        free(header);
        return;
    }
    mtx_unlock(&pool_mutex);
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
