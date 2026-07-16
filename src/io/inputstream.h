#ifndef CDF_IO_INPUTSTREAM_H
#define CDF_IO_INPUTSTREAM_H

#include "../core/core.h"


typedef struct {
    inherits(Object);
    int (*read)(ObjectPtr);
} InputStream;

InputStream * InputStream_new(InputStream *);
void InputStream_delete(ObjectPtr);


#endif

