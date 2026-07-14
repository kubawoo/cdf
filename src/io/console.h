#ifndef CDF_IO_CONSOLE_H
#define CDF_IO_CONSOLE_H

#include "../core/core.h"


typedef struct {
    inherits(Object);
    void (*print_object)(ObjectPtr, ObjectPtr);
    void (*print_cstring)(ObjectPtr, const char *);
} Console;

Console * Console_new(Console *);
void Console_delete(ObjectPtr);


#endif

