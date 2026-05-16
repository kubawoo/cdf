#include <sqlite3.h>
#include "sqlite_database.h"
#include "sqlite_connection.h"

static String * to_string(ObjectPtr _this) {
	make_this(SQLiteDatabase, _this);
	String * s = new(String);
	call(s, format, "SQLite v%s; name=%s\n", sqlite3_libversion(), call(this->_name, to_cstring));
	return s;
}

static DbConnection * open(ObjectPtr _this) {
	make_this(SQLiteDatabase, _this);
	sqlite3 *db;
	int rc = sqlite3_open(call(this->_name, to_cstring), &db);
	SQLiteConnection * conn = NULL;
	if(rc == SQLITE_OK) {
		conn = new(SQLiteConnection, db);
	} else {
		sqlite3_close(db);
	}
	return (DbConnection *)conn;
}

SQLiteDatabase * SQLiteDatabase_new(SQLiteDatabase * this) {
	String * memory = new(String, ":memory:");
    this(SQLiteDatabase, memory);
    REFCDEC(memory);
    return this;
}

SQLiteDatabase * SQLiteDatabase_new1(SQLiteDatabase * this, String * name) {
    super(Database, SQLiteDatabase);
    override(Object, to_string, to_string);
    override(Database, to_string, to_string);
    override(Database, open, open);
    REFCINC(name);
    this->_name = name;
    return this;
}

void SQLiteDatabase_delete(ObjectPtr _this) {
	make_this(SQLiteDatabase, _this);
	REFCDEC(this->_name);
    super_delete(Database, _this);
}

