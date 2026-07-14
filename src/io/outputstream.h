#ifndef CDF_IO_OUTPUTSTREAM_H
#define CDF_IO_OUTPUTSTREAM_H

#include "../core/core.h"


typedef struct {
    inherits(Object);
    void (*write)(ObjectPtr, char);
    void (*write_string)(ObjectPtr, String *);
    String * (*to_string)(ObjectPtr);
} OutputStream;

OutputStream * OutputStream_new(OutputStream *);
void OutputStream_delete(ObjectPtr);


#endif

