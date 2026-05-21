#ifndef OOC_COLLECTION_H
#define OOC_COLLECTION_H

#include "ooc_object.h"

// Forward declaration
typedef struct _Iterator Iterator;

typedef struct {
    inherits(Object);
    Iterator* (*iterator)(ObjectPtr);
} Collection;

Collection * Collection_new(Collection * this);
void Collection_delete(ObjectPtr);

#endif