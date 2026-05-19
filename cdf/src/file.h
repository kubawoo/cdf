#ifndef FILE_H
#define FILE_H

#include "ooc.h"
#include <stdio.h>

typedef struct {
    inherits(Object);
    bool (*open)(ObjectPtr, String * mode);
    void (*close)(ObjectPtr);
    bool (*is_open)(ObjectPtr);
    bool (*exists)(ObjectPtr);
    int (*read_char)(ObjectPtr);
    String * (*read)(ObjectPtr);
    bool (*write_char)(ObjectPtr, char);
    bool (*write_cstring)(ObjectPtr, const char *);
    bool (*write_string)(ObjectPtr, String *);
    bool (*flush)(ObjectPtr);
    FILE * _fp;
    String * _path;
} File;

File * File_new1(File *, String * path);
void File_delete(ObjectPtr);

#endif
