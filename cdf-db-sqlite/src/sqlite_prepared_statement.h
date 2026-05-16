#ifndef SQLITE_PREPARED_STATEMENT_H
#define SQLITE_PREPARED_STATEMENT_H

#include <ooc.h>
#include <prepared_statement.h>
#include <sqlite3.h>

typedef struct {
    inherits(DbPreparedStatement);
    sqlite3_stmt * _stmt;
} SQLitePreparedStatement;

SQLitePreparedStatement * SQLitePreparedStatement_new2(SQLitePreparedStatement *, sqlite3 *,String *);
void SQLitePreparedStatement_delete(ObjectPtr);

#endif

