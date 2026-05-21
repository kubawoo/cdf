#ifndef OOC_ARRAY_H
#define OOC_ARRAY_H

#include "ooc_collection.h"
#include "ooc_iterator.h"
#include "ooc_object.h"

typedef struct {
    inherits(Collection);
    int length;
    Object ** _values;
    void (*set)(ObjectPtr, int, ObjectPtr);
    ObjectPtr (*get)(ObjectPtr, int);
} Array;

// Iterator implementation for Array
typedef struct {
    inherits(Iterator);
    Array * array;
    int index;
} ArrayIterator;

Array * Array_new1(Array * this, int size);
void Array_delete(ObjectPtr);
ArrayIterator * ArrayIterator_new(ArrayIterator * this, Array * array);
bool ArrayIterator_hasNext(ObjectPtr _this);
ObjectPtr ArrayIterator_next(ObjectPtr _this);

#endif