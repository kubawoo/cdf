#ifndef PARSER_HANDLERS_H
#define PARSER_HANDLERS_H

#include "eventparser.h"
#include "jsonobject.h"
#include "../core/cdf.h"

typedef struct  {
    inherits(JsonEventsHandler);
    void (*object_begin)(ObjectPtr, String *);
    void (*object_end)(ObjectPtr);
    void (*array_begin)(ObjectPtr, String *);
    void (*array_end)(ObjectPtr);
    void (*value)(ObjectPtr, String *, Object *);

    JsonObject * object;
    Stack * _stack;
} JsonObjectBuilderEventsHandler;

JsonObjectBuilderEventsHandler * JsonObjectBuilderEventsHandler_new(JsonObjectBuilderEventsHandler *);
void JsonObjectBuilderEventsHandler_delete(ObjectPtr);


#endif
