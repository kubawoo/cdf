#ifndef CJSON_EVENT_PARSER_H
#define CJSON_EVENT_PARSER_H

#include <ooc.h>
#include <io.h>

typedef struct {
    inherits(Object);
    void (*object_begin)(ObjectPtr, String *);
    void (*object_end)(ObjectPtr);
    void (*array_begin)(ObjectPtr, String *);
    void (*array_end)(ObjectPtr);
    void (*value)(ObjectPtr, String *, Object*);
} JsonEventsHandler;

JsonEventsHandler * JsonEventsHandler_new(JsonEventsHandler *);
void JsonEventsHandler_delete(ObjectPtr);

#define CJSON_PARSE_SUCCESS 0
#define CJSON_PARSE_NO_HANDLER 1
#define CJSON_PARSE_NO_INPUT 2
#define CJSON_PARSE_INVALID_JSON 3
#define CJSON_PARSE_INVALID_VALUE 4


typedef struct {
    inherits(Object);
    Stack * _states;
    JsonEventsHandler * _handler;
    String * _buffer;
    String * _name;
    String * _value;

    int (*parse)(ObjectPtr, InputStream * json_stream);
} JsonEventsParser;

JsonEventsParser * JsonEventsParser_new1(JsonEventsParser *, JsonEventsHandler *);
void JsonEventsParser_delete(ObjectPtr);


#endif

