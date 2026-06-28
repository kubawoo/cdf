#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

#include "database.h"

typedef struct {
    inherits(Database);
    String * (*to_string)(ObjectPtr);
    DbConnection * (*open)(ObjectPtr);
    String * _name;
} SQLiteDatabase;

SQLiteDatabase * SQLiteDatabase_new(SQLiteDatabase *);
SQLiteDatabase * SQLiteDatabase_new1(SQLiteDatabase *, String *);
void SQLiteDatabase_delete(ObjectPtr);


#endif

