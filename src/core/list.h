#ifndef CDF_CORE_LIST_H
#define CDF_CORE_LIST_H

#include "collection.h"
#include "iterator.h"

typedef struct _ListItem {
    inherits(Object);
    Object * _item;
    struct _ListItem * _next;
    struct _ListItem * _prev;
} _ListItem;

_ListItem * _ListItem_new1(_ListItem * this, Object *);
_ListItem * _ListItem_new2(_ListItem * this, Object *, _ListItem * prev);
_ListItem * _ListItem_new3(_ListItem * this, Object *, _ListItem * prev, _ListItem * next);
void _ListItem_delete(ObjectPtr);

typedef struct {
    inherits(Collection);
    _ListItem * _first;
    _ListItem * _last;
    int _length;

    void (*add)(ObjectPtr, ObjectPtr);
    void (*remove)(ObjectPtr, int);
    ObjectPtr (*get)(ObjectPtr, int);
    void (*set)(ObjectPtr, int, ObjectPtr);
    void (*insert)(ObjectPtr, int, ObjectPtr);
    bool (*contains)(ObjectPtr, ObjectPtr);

    String * (*to_string)(ObjectPtr);
    Iterator* (*iterator)(ObjectPtr);
    unsigned int (*size)(ObjectPtr);
} List;

List * List_new(List * this);
void List_delete(ObjectPtr);


// Iterator implementation for List
typedef struct {
    inherits(Iterator);
    bool (*hasNext)(ObjectPtr);
    ObjectPtr (*next)(ObjectPtr);
    List * _list;
    _ListItem * _curr;
} ListIterator;


ListIterator * ListIterator_new1(ListIterator * this, List * list);
void ListIterator_delete(ObjectPtr);

#endif