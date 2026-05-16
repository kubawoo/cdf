#ifndef OOC_STRING_H
#define OOC_STRING_H

#include "ooc_object.h"

struct _String {
    inherits(Object);
    unsigned int length;
    void (*set_text)(void *, const char *);
    const char * (*to_cstring)(void *);
    String * (*to_string)(void *);
    bool (*equals)(void *, void *);
    ObjectPtr (*copy)(void *);
    bool (*equals_cstring)(void *, const char *);
    void (*append_cstring)(void *, const char *);
    void (*append)(void *, String *);
    void (*append_char)(void *, const char);
    void (*clear)(void *);
    void (*trim)(void *);
    bool (*contains_any)(void *, String *);
    bool (*contains_any_char)(void *, const char *);
    int (*index_of_char)(void *, const char);
    int (*next_index_of_char)(void *, int, const char);
    int (*index_of_string)(void *, String *);
    int (*next_index_of_string)(void *, int, String *);
    int (*index_of_cstring)(void * , const char *);
    int (*next_index_of_cstring)(void *, int, const char *);
    String * (*substring)(void *, int, int);
    String * (*substring_from)(void *, int);
    void (*format)(ObjectPtr, const char *, ...);
    const char (*char_at)(ObjectPtr, int);

    //'private'
    char * _content;
    unsigned int _allocated;
};


String * String_new(String *);
String * String_new1(String *, const char *);
void String_delete(void *);

#endif

