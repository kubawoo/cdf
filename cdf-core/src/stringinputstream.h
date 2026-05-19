#ifndef STRINGINPUTSTREAM_H
#define STRINGINPUTSTREAM_H

#include "inputstream.h"


typedef struct {
    inherits(InputStream);
    int (*read)(ObjectPtr);
    String * _txt;
    int _pos;
} StringInputStream;

StringInputStream * StringInputStream_new1(StringInputStream *, String * txt);
void StringInputStream_delete(ObjectPtr);


#endif

