#ifndef OOC_MEMORY_H
#define OOC_MEMORY_H

#include <stddef.h>

extern size_t pool_allocs;
extern size_t pool_reuses;

void * pool_alloc(size_t size);
void   pool_free(void *ptr);
void   pool_cleanup(void);

#endif
