#ifndef OOC_STRINGTOKENIZER_H
#define OOC_STRINGTOKENIZER_H

#include "ooc_string.h"
#include "ooc_list.h"

typedef struct _StringTokenizer {
    inherits(Object);
    List* (*split_by_char)(ObjectPtr, const char);
    //'private'
    String * _content;
} StringTokenizer;


StringTokenizer * StringTokenizer_new1(StringTokenizer * this, String *);
void StringTokenizer_delete(ObjectPtr);

#endif

