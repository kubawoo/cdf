#ifndef OOC_ITERATOR_H
#define OOC_ITERATOR_H

#include "ooc_object.h"

typedef struct _Iterator {
    inherits(Object);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
} Iterator;

Iterator * Iterator_new(Iterator * this);
void Iterator_delete(ObjectPtr);

#endif