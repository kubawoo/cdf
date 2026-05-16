#ifndef CONSOLE_H
#define CONSOLE_H

#include <ooc.h>


typedef struct {
    inherits(Object);
    void (*print_object)(ObjectPtr, ObjectPtr);
} Console;

Console * Console_new(Console *);
void Console_delete(ObjectPtr);


#endif

