#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

#include "inputstream.h"
#include <stdio.h>

typedef struct {
    inherits(InputStream);
    int (*read)(ObjectPtr);
    FILE * _file;
} FileInputStream;

FileInputStream * FileInputStream_new1(FileInputStream *, String * filename);
void FileInputStream_delete(ObjectPtr);


#endif

