#include "sqlite_prepared_statement.h"
#include <stdio.h>

SQLitePreparedStatement * SQLitePreparedStatement_new2(SQLitePreparedStatement * this, sqlite3 * db, String * sql) {
	super(DbPreparedStatement, SQLitePreparedStatement);
	int rc = sqlite3_prepare_v2(db, call(sql, to_cstring), -1, &(this->_stmt), NULL);
	if(rc != SQLITE_OK) {
		REFCDEC(this);
		return NULL;
	}
	return this;
}

void SQLitePreparedStatement_delete(ObjectPtr _this) {
	make_this(SQLitePreparedStatement, _this);
	sqlite3_finalize(this->_stmt);
	super_delete(DbPreparedStatement, _this);
}
