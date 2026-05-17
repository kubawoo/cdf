#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <ooc.h>

typedef struct {
    inherits(Object);
    bool (*create)(ObjectPtr, String * path);
} Directory;

Directory * Directory_new(Directory *);
void Directory_delete(ObjectPtr);

#endif
