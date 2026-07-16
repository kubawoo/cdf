#ifndef CDF_IO_STRINGOUTPUTSTREAM_H
#define CDF_IO_STRINGOUTPUTSTREAM_H

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

