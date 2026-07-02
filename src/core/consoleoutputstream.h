#ifndef CONSOLEOUTPUTSTREAM_H
#define CONSOLEOUTPUTSTREAM_H

#include "outputstream.h"


typedef struct {
    inherits(OutputStream);
    void (*write)(ObjectPtr, char);
} ConsoleOutputStream;

ConsoleOutputStream * ConsoleOutputStream_new(ConsoleOutputStream *);
void ConsoleOutputStream_delete(ObjectPtr);

#endif

