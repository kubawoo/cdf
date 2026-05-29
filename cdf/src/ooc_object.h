#ifndef OOC_OBJECT_H
#define OOC_OBJECT_H

#include "ooc_macros.h"
#include <stdbool.h>

typedef struct _String  String;

typedef struct _Object {
    void (*_ooc_destructor)(ObjectPtr);
    _Atomic int _refc;
    const char * type;
    bool (*equals)(ObjectPtr, ObjectPtr);
    String * (*to_string)(ObjectPtr);
    ObjectPtr (*copy)(ObjectPtr);
} Object;


Object * Object_new(Object *);
void Object_delete(ObjectPtr);

void * _refctmp(void * o);

#include "ooc_string.h"

#endif

