#ifndef OOC_ARRAY_H
#define OOC_ARRAY_H

#include "ooc_object.h"

typedef struct {
    inherits(Object);
    int length;
    Object ** _values;
    void (*set)(ObjectPtr, int, ObjectPtr);
    ObjectPtr (*get)(ObjectPtr, int);
} Array;

Array * Array_new1(Array * this, int);
void Array_delete(ObjectPtr);


#endif
