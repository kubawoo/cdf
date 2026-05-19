#ifndef STRINGOUTPUTSTREAM_H
#define STRINGOUTPUTSTREAM_H

#include "outputstream.h"


typedef struct {
    inherits(OutputStream);
    void (*write)(ObjectPtr, char);
    String * _txt;
    String * (*to_string)(ObjectPtr);
} StringOutputStream;

StringOutputStream * StringOutputStream_new(StringOutputStream *);
void StringOutputStream_delete(ObjectPtr);

#endif

