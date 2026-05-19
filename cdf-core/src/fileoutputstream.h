#ifndef FILEOUTPUTSTREAM_H
#define FILEOUTPUTSTREAM_H

#include "outputstream.h"
#include "file.h"


typedef struct {
    inherits(OutputStream);
    void (*write)(ObjectPtr, char);
    File * _file;
} FileOutputStream;

FileOutputStream * FileOutputStream_new1(FileOutputStream *, String * filename);
FileOutputStream * FileOutputStream_new2(FileOutputStream *, String * filename, bool append);
FileOutputStream * FileOutputStream_new3(FileOutputStream *, File * file, bool append);
void FileOutputStream_delete(ObjectPtr);

#endif

