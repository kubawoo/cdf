#ifndef PARSER_HANDLERS_H
#define PARSER_HANDLERS_H

#include "eventparser.h"
#include "jsonobject.h"

typedef struct  {
    inherits(JsonEventsHandler);
    void (*object_begin)(ObjectPtr, String *);
    void (*object_end)(ObjectPtr);
    void (*array_begin)(ObjectPtr, String *);
    void (*array_end)(ObjectPtr);
    void (*value)(ObjectPtr, String *, Object *);

    /* The root object, or NULL when the document was a top-level array. */
    JsonObject * (*get_object)(ObjectPtr);

    /* The root array, or NULL when the document was a top-level object. */
    List * (*get_list)(ObjectPtr);

    JsonObject * _object;
    List * _list;
    Stack * _stack;
} JsonObjectBuilderEventsHandler;

JsonObjectBuilderEventsHandler * JsonObjectBuilderEventsHandler_new(JsonObjectBuilderEventsHandler *);
void JsonObjectBuilderEventsHandler_delete(ObjectPtr);


#endif
