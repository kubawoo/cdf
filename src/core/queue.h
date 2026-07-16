#ifndef CDF_CORE_QUEUE_H
#define CDF_CORE_QUEUE_H

#include "collection.h"
#include "list.h"


typedef struct {
    inherits(Collection);
    List * _list;

    void (*enqueue)(ObjectPtr, ObjectPtr);
    ObjectPtr (*dequeue)(ObjectPtr);
    ObjectPtr (*peek)(ObjectPtr);

    String * (*to_string)(ObjectPtr);
    Iterator* (*iterator)(ObjectPtr);
    unsigned int (*size)(ObjectPtr);
} Queue;

Queue * Queue_new(Queue * this);
void Queue_delete(ObjectPtr);

typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    List * _list;
    int _index;
} QueueIterator;

QueueIterator * QueueIterator_new1(QueueIterator * this, List * list);
void QueueIterator_delete(ObjectPtr);

#endif
