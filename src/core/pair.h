#ifndef CDF_CORE_PAIR_H
#define CDF_CORE_PAIR_H

#include "object.h"

typedef struct {
    inherits(Object);
    Object * _left;
    Object * _right;

    ObjectPtr (*left)(ObjectPtr);
    ObjectPtr (*right)(ObjectPtr);

    String * (*to_string)(ObjectPtr);
} Pair;

Pair * Pair_new2(Pair * this, ObjectPtr left, ObjectPtr right);
void Pair_delete(ObjectPtr);

#endif

