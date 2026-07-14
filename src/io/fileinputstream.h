#ifndef CDF_IO_FILEINPUTSTREAM_H
#define CDF_IO_FILEINPUTSTREAM_H

#include "inputstream.h"
#include "file.h"

typedef struct {
    inherits(InputStream);
    int (*read)(ObjectPtr);
    File * _file;
} FileInputStream;

FileInputStream * FileInputStream_new1(FileInputStream *, File * file);
void FileInputStream_delete(ObjectPtr);


#endif

