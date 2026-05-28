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
    Iterator* (*iterator)(ObjectPtr);
} Array;

Array * Array_new1(Array * this, int size);
void Array_delete(ObjectPtr);

// Iterator implementation for Array
typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    Array * array;
    int index;
} ArrayIterator;


ArrayIterator * ArrayIterator_new1(ArrayIterator * this, Array * array);
void ArrayIterator_delete(ObjectPtr);

#endif