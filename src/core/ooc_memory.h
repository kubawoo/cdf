#ifndef OOC_MEMORY_H
#define OOC_MEMORY_H

#include <stddef.h>

void * pool_alloc(size_t size);
void   pool_free(void *ptr);
void   pool_cleanup(void);

#endif
