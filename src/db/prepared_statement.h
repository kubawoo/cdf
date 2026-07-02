#ifndef DB_PREPARED_STATEMENT_H
#define DB_PREPARED_STATEMENT_H

#include "../core/ooc.h"

typedef struct {
	inherits(Object);
} DbPreparedStatement;

DbPreparedStatement * DbPreparedStatement_new(DbPreparedStatement *);
void DbPreparedStatement_delete(ObjectPtr);

#endif
