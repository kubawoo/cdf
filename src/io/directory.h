#ifndef CDF_IO_DIRECTORY_H
#define CDF_IO_DIRECTORY_H

#include "../core/core.h"

typedef struct {
    inherits(Object);
    bool (*create)(ObjectPtr, String * path);
} Directory;

Directory * Directory_new(Directory *);
void Directory_delete(ObjectPtr);

#endif
