#ifndef CDF_DB_DATABASE_H
#define CDF_DB_DATABASE_H

#include "../core/core.h"
#include "connection.h"

typedef struct {
    inherits(Object);
    String * (*to_string)(ObjectPtr);
    DbConnection * (*open)(ObjectPtr);
} Database;

Database * Database_new(Database *);
void Database_delete(ObjectPtr);


#endif
