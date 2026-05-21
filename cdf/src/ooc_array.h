#ifndef OOC_ARRAY_H
#define OOC_ARRAY_H

#include "ooc_object.h"
#include "ooc_collection.h"

/* Forward declaration */
typedef struct _ArrayIterator ArrayIterator;

typedef struct {
    inherits(Collection);
    int length;
    Object ** _values;
    void (*set)(ObjectPtr, int, ObjectPtr);
    ObjectPtr (*get)(ObjectPtr, int);
    Iterator * (*iterator)(ObjectPtr);
} Array;

Array * Array_new1(Array * this, int);
void Array_delete(ObjectPtr);


#endif