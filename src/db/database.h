#ifndef DATABASE_H
#define DATABASE_H

#include "../core/ooc.h"
#include "connection.h"

typedef struct {
    inherits(Object);
    String * (*to_string)(ObjectPtr);
    DbConnection * (*open)(ObjectPtr);
} Database;

Database * Database_new(Database *);
void Database_delete(ObjectPtr);


#endif
