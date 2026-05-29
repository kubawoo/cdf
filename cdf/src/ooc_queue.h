#ifndef OOC_QUEUE_H
#define OOC_QUEUE_H

#include "ooc_collection.h"
#include "ooc_list.h"


typedef struct {
    inherits(Collection);
    List * _list;

    void (*enqueue)(ObjectPtr, ObjectPtr);
    ObjectPtr (*dequeue)(ObjectPtr);
    ObjectPtr (*peek)(ObjectPtr);
    int (*size)(ObjectPtr);

    String * (*to_string)(ObjectPtr);
    Iterator* (*iterator)(ObjectPtr);
} Queue;

Queue * Queue_new(Queue * this);
void Queue_delete(ObjectPtr);

typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    List * list;
    int index;
} QueueIterator;

QueueIterator * QueueIterator_new1(QueueIterator * this, List * list);
void QueueIterator_delete(ObjectPtr);

#endif
