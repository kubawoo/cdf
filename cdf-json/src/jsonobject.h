#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include <cdf.h>


typedef struct {
    inherits(Object);
    Map * _map;
    String * (*to_string)(ObjectPtr);
    void (*put_value)(ObjectPtr, String *, ObjectPtr);
    ObjectPtr (*get_value)(ObjectPtr, String *);
    Map * (*get_map)(ObjectPtr);
} JsonObject;

JsonObject * JsonObject_new(JsonObject *);
void JsonObject_delete(ObjectPtr);


#endif

