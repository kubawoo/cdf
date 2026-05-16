#ifndef FILEOUTPUTSTREAM_H
#define FILEOUTPUTSTREAM_H

#include "outputstream.h"
#include <stdio.h>


typedef struct {
    inherits(OutputStream);
    void (*write)(ObjectPtr, char);
    FILE * _file;
} FileOutputStream;

FileOutputStream * FileOutputStream_new1(FileOutputStream *, String * filename);
FileOutputStream * FileOutputStream_new2(FileOutputStream *, String * filename, bool append);
void FileOutputStream_delete(ObjectPtr);

#endif

