#ifndef OOC_PAIR_H
#define OOC_PAIR_H

#include "ooc_object.h"

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

