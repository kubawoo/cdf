#ifndef OOC_STACK_H
#define OOC_STACK_H

#include "ooc_list.h"


typedef struct {
    inherits(Object);
    List * _list;

    void (*push)(ObjectPtr, ObjectPtr);
    ObjectPtr (*pop)(ObjectPtr);
    ObjectPtr (*peek)(ObjectPtr);
    int (*size)(ObjectPtr);

    String * (*to_string)(ObjectPtr);
} Stack;

Stack * Stack_new(Stack * this);
void Stack_delete(ObjectPtr);

#endif

