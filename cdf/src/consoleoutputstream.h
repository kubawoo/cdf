#ifndef STRINGOUTPUTSTREAM_H
#define STRINGOUTPUTSTREAM_H

#include "outputstream.h"


typedef struct {
    inherits(OutputStream);
    void (*write)(ObjectPtr, char);
} ConsoleOutputStream;

ConsoleOutputStream * ConsoleOutputStream_new(ConsoleOutputStream *);
void ConsoleOutputStream_delete(ObjectPtr);

#endif

