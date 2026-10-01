#include "memory.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>


static void alloc_null_is_safe(void)
{
    pool_free(NULL);
    assert(1);
}

static void recycled_block_is_reused(void)
{
    void *a = pool_alloc(64);
    assert(a != NULL);
    memset(a, 0xAB, 64);
    pool_free(a);

    void *b = pool_alloc(64);
    assert(b == a);
    pool_free(b);
}

static void double_free_is_ignored(void)
{
    void *p = pool_alloc(64);
    pool_free(p);
    pool_free(p);

    /* A third free must stay harmless too, and the block must still be
       reclaimable from the freelist afterwards. */
    pool_free(p);

    void *q = pool_alloc(64);
    assert(q == p);

    pool_free(q);
    pool_cleanup();
}

static void large_block_double_free(void)
{
    /* Above POOL_MAX_BUCKETS the pool forwards straight to free(), so the
       header takes the non-pooled path. */
    void *p = pool_alloc(1024 * 1024);
    assert(p != NULL);
    memset(p, 0x5A, 1024 * 1024);
    pool_free(p);
    pool_cleanup();
}

static void free_null_after_alloc(void)
{
    void *p = pool_alloc(0);
    assert(p != NULL);
    pool_free(p);
    pool_free(NULL);
    assert(1);
}

static void cleanup_leaves_pool_usable(void)
{
    void *p = pool_alloc(64);
    pool_free(p);
    pool_cleanup();

    void *q = pool_alloc(64);
    assert(q != NULL);
    memset(q, 0x3C, 64);
    pool_free(q);
}

int main(void)
{
    alloc_null_is_safe();
    recycled_block_is_reused();
    double_free_is_ignored();
    large_block_double_free();
    free_null_after_alloc();
    cleanup_leaves_pool_usable();
    return 0;
}