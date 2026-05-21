#ifndef OOC_QUEUE_H
#define OOC_QUEUE_H

#include "ooc_list.h"


typedef struct {
    inherits(Object);
    List * _list;

    void (*enqueue)(ObjectPtr, ObjectPtr);
    ObjectPtr (*dequeue)(ObjectPtr);
    ObjectPtr (*peek)(ObjectPtr);
    int (*size)(ObjectPtr);

    String * (*to_string)(ObjectPtr);
} Queue;

Queue * Queue_new(Queue * this);
void Queue_delete(ObjectPtr);

#endif
