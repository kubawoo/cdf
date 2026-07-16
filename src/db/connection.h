#ifndef CDF_DB_CONNECTION_H
#define CDF_DB_CONNECTION_H

#include "../core/core.h"
#include "prepared_statement.h"

typedef struct {
	inherits(Object);
	bool (*exec)(ObjectPtr, String *);
	List * (*query)(ObjectPtr, String *);
	String * (*last_error_msg)(ObjectPtr);
	int (*last_error_code)(ObjectPtr);
	long (*last_inserted_id)(ObjectPtr);
	void (*close)(ObjectPtr);
	DbPreparedStatement * (*prepare_statement)(ObjectPtr, String *);
	bool (*exec_statement)(ObjectPtr, DbPreparedStatement *, List *);
	List * (*query_statement)(ObjectPtr, DbPreparedStatement *, List *);

} DbConnection;

DbConnection * DbConnection_new(DbConnection *);
void DbConnection_delete(ObjectPtr);

#endif
