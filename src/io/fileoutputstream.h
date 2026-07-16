#ifndef CDF_IO_FILEOUTPUTSTREAM_H
#define CDF_IO_FILEOUTPUTSTREAM_H

#include "outputstream.h"
#include "file.h"


typedef struct {
    inherits(OutputStream);
    void (*write)(ObjectPtr, char);
    File * _file;
} FileOutputStream;

FileOutputStream * FileOutputStream_new1(FileOutputStream *, File * file);
FileOutputStream * FileOutputStream_new2(FileOutputStream * this, File * file, bool append);
void FileOutputStream_delete(ObjectPtr);

#endif

