#ifndef CDF_DB_PREPARED_STATEMENT_H
#define CDF_DB_PREPARED_STATEMENT_H

#include "../core/core.h"

typedef struct {
	inherits(Object);
} DbPreparedStatement;

DbPreparedStatement * DbPreparedStatement_new(DbPreparedStatement *);
void DbPreparedStatement_delete(ObjectPtr);

#endif
