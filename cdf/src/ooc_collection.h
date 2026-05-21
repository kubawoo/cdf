#ifndef OOC_COLLECTION_H
#define OOC_COLLECTION_H

#include "ooc_object.h"

/* Forward declaration */
typedef struct _Iterator Iterator;

/* Collection base class */
typedef struct {
    inherits(Object);
    /* Returns an iterator for traversing the collection */
    Iterator * (*iterator)(ObjectPtr);
} Collection;

/* Iterator interface */
typedef struct _Iterator {
    inherits(Object);
    /* Returns true if there are more elements */
    bool (*has_next)(ObjectPtr);
    /* Returns the next element and advances the iterator */
    ObjectPtr (*next)(ObjectPtr);
} Iterator;

/* Collection virtual destructor */
void Collection_delete(ObjectPtr);

/* Iterator virtual destructor */
void Iterator_delete(ObjectPtr);

/* Iterator constructor */
Iterator * Iterator_new(Iterator * this);

#endif