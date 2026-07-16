#ifndef CDF_CORE_OBJECT_H
#define CDF_CORE_OBJECT_H

#include "macros.h"
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

#include "thestring.h"

#endif

