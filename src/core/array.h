#ifndef CDF_CORE_ARRAY_H
#define CDF_CORE_ARRAY_H

#include "collection.h"
#include "iterator.h"
#include "object.h"

typedef struct {
    inherits(Collection);
    int _length;
    Object ** _values;

    void (*set)(ObjectPtr, int, ObjectPtr);
    ObjectPtr (*get)(ObjectPtr, int);
    Iterator* (*iterator)(ObjectPtr);
    unsigned int (*size)(ObjectPtr);
} Array;

Array * Array_new1(Array * _this, int size);
void Array_delete(ObjectPtr);

// Iterator implementation for Array
typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    Array * _array;
    int _index;
} ArrayIterator;


ArrayIterator * ArrayIterator_new1(ArrayIterator * _this, Array * array);
void ArrayIterator_delete(ObjectPtr);

#endif