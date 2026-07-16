#ifndef CDF_CORE_STRINGTOKENIZER_H
#define CDF_CORE_STRINGTOKENIZER_H

#include "string.h"
#include "list.h"

typedef struct _StringTokenizer {
    inherits(Object);
    List* (*split_by_char)(ObjectPtr, const char);
    //'private'
    String * _content;
} StringTokenizer;


StringTokenizer * StringTokenizer_new1(StringTokenizer * this, String *);
void StringTokenizer_delete(ObjectPtr);

#endif

