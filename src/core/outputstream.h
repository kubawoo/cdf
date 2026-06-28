#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include "ooc.h"


typedef struct {
    inherits(Object);
    void (*write)(ObjectPtr, char);
    void (*write_string)(ObjectPtr, String *);
    String * (*to_string)(ObjectPtr);
} OutputStream;

OutputStream * OutputStream_new(OutputStream *);
void OutputStream_delete(ObjectPtr);


#endif

