#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

#include "inputstream.h"
#include "file.h"

typedef struct {
    inherits(InputStream);
    int (*read)(ObjectPtr);
    File * _file;
} FileInputStream;

FileInputStream * FileInputStream_new1(FileInputStream *, String * filename);
FileInputStream * FileInputStream_new2(FileInputStream *, File * file);
void FileInputStream_delete(ObjectPtr);


#endif

