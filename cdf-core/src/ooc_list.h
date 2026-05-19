#ifndef OOC_LIST_H
#define OOC_LIST_H

#include "ooc_object.h"

typedef struct _ListItem {
    inherits(Object);
    Object * item;
    struct _ListItem * next;
    struct _ListItem * prev;
} _ListItem;

_ListItem * _ListItem_new1(_ListItem * this, Object *);
_ListItem * _ListItem_new2(_ListItem * this, Object *, _ListItem * prev);
_ListItem * _ListItem_new3(_ListItem * this, Object *, _ListItem * prev, _ListItem * next);
void _ListItem_delete(ObjectPtr);

typedef struct {
    inherits(Object);
    _ListItem * _first;
    _ListItem * _last;
    int length;

    void (*add)(ObjectPtr, ObjectPtr);
    void (*remove)(ObjectPtr, int);
    ObjectPtr (*get)(ObjectPtr, int);
    void (*set)(ObjectPtr, int, ObjectPtr);
    void (*insert)(ObjectPtr, int, ObjectPtr);
    bool (*contains)(ObjectPtr, ObjectPtr);

    String * (*to_string)(ObjectPtr);
} List;

List * List_new(List * this);
void List_delete(ObjectPtr);

#endif

