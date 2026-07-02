#ifndef SQLITE_CONNECTION_H
#define SQLITE_CONNECTION_H

#include "../db/database.h"
#include <sqlite3.h>

typedef struct {
	inherits(DbConnection);
	bool (*exec)(ObjectPtr, String *);
	List * (*query)(ObjectPtr, String *);
	String * (*last_error_msg)(ObjectPtr);
	int (*last_error_code)(ObjectPtr);
	long (*last_inserted_id)(ObjectPtr);
	void (*close)(ObjectPtr);
	DbPreparedStatement * (*prepare_statement)(ObjectPtr, String *);
	bool (*exec_statement)(ObjectPtr, DbPreparedStatement *, List *);
	List * (*query_statement)(ObjectPtr, DbPreparedStatement *, List *);

	sqlite3 *_db;
} SQLiteConnection;

SQLiteConnection * SQLiteConnection_new1(SQLiteConnection *, sqlite3 *);
void SQLiteConnection_delete(ObjectPtr);

#endif
