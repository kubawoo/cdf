#include "ooc_memory.h"
#include <stdlib.h>

#define POOL_MAX_BUCKETS 128

size_t pool_allocs;
size_t pool_reuses;

static void *freelists[POOL_MAX_BUCKETS];

void *pool_alloc(size_t size) {
    size_t aligned = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    int idx = aligned / sizeof(void*);

    if (idx < POOL_MAX_BUCKETS) {
        void *obj = freelists[idx];
        if (obj) {
            freelists[idx] = *(void**)obj;
            pool_reuses++;
            return obj;
        }
    }

    size_t *header = malloc(sizeof(size_t) + aligned);
    pool_allocs++;
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
        *(void **)ptr = freelists[idx];
        freelists[idx] = ptr;
    } else {
        free(header);
    }
}

__attribute__((destructor))
static void pool_auto_cleanup(void) {
    pool_cleanup();
}

void pool_cleanup(void) {
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
}
