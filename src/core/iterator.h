#ifndef CDF_CORE_ITERATOR_H
#define CDF_CORE_ITERATOR_H

#include "object.h"

typedef struct _Iterator {
    inherits(Object);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
} Iterator;

Iterator * Iterator_new(Iterator * this);
void Iterator_delete(ObjectPtr);

#endif