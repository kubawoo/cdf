#ifndef CDF_CORE_STACK_H
#define CDF_CORE_STACK_H

#include "collection.h"
#include "list.h"


typedef struct {
    inherits(Collection);
    List * _list;

    void (*push)(ObjectPtr, ObjectPtr);
    ObjectPtr (*pop)(ObjectPtr);
    ObjectPtr (*peek)(ObjectPtr);
    int (*size)(ObjectPtr);

    String * (*to_string)(ObjectPtr);
    Iterator* (*iterator)(ObjectPtr);
} Stack;

Stack * Stack_new(Stack * this);
void Stack_delete(ObjectPtr);

typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    List * list;
    int index;
} StackIterator;

StackIterator * StackIterator_new1(StackIterator * this, List * list);
void StackIterator_delete(ObjectPtr);

#endif
