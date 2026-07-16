#ifndef CDF_IO_CONSOLEOUTPUTSTREAM_H
#define CDF_IO_CONSOLEOUTPUTSTREAM_H

#include "outputstream.h"


typedef struct {
    inherits(OutputStream);
    void (*write)(ObjectPtr, char);
} ConsoleOutputStream;

ConsoleOutputStream * ConsoleOutputStream_new(ConsoleOutputStream *);
void ConsoleOutputStream_delete(ObjectPtr);

#endif

