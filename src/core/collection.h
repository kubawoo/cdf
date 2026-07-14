#ifndef CDF_CORE_COLLECTION_H
#define CDF_CORE_COLLECTION_H

#include "object.h"
#include "iterator.h"

typedef struct {
    inherits(Object);
    Iterator* (*iterator)(ObjectPtr);
} Collection;

Collection * Collection_new(Collection * this);
void Collection_delete(ObjectPtr);

#endif