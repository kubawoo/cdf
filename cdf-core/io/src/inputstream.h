#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <ooc.h>


typedef struct {
    inherits(Object);
    int (*read)(ObjectPtr);
} InputStream;

InputStream * InputStream_new(InputStream *);
void InputStream_delete(ObjectPtr);


#endif

