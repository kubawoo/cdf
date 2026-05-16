#include <sqlite3.h>
#include <stdio.h>
#include "sqlite_connection.h"
#include "sqlite_prepared_statement.h"

static bool exec(ObjectPtr _this, String * sql) {
	make_this(SQLiteConnection, _this);
	int rc = sqlite3_exec(this->_db, call(sql, to_cstring), NULL, NULL, NULL);
	if(rc != SQLITE_OK) {
		return false;
	}
	return true;
}

static void close(ObjectPtr _this) {
	make_this(SQLiteConnection, _this);
	sqlite3_close(this->_db);
	this->_db = NULL;
}

static Object * get_value(sqlite3_stmt * res, int i) {
	int type = sqlite3_column_type(res, i);
	Object * ret_value = NULL;
	switch(type) {
	case SQLITE_INTEGER:
		ret_value = (Object *) new(Integer, sqlite3_column_int(res, i));
		break;
	case SQLITE_TEXT:
		ret_value = (Object *) new(String, (const char *)sqlite3_column_text(res, i));
		break;
	case SQLITE_FLOAT:
		ret_value = (Object *) new(Double, sqlite3_column_double(res, i));
		break;
	case SQLITE_BLOB:
		break;
	}
	return ret_value;
}


static DbPreparedStatement * prepare_statement(ObjectPtr _this, String * sql) {
	make_this(SQLiteConnection, _this);
	return new(SQLitePreparedStatement, this->_db, sql);
}

static sqlite3_stmt * bind_statement(DbPreparedStatement * _stmt, List * bindings) {
	sqlite3_stmt * stmt = ((SQLitePreparedStatement *) _stmt)->_stmt;
	sqlite3_reset(stmt);
	sqlite3_clear_bindings(stmt);

	if(bindings != NULL) {
		for(int i = 0; i < bindings->length; ++i) {
			Object * binding = call(bindings, get, i);
			if(type_equal(binding, "String")) {
				String * s = (String *) binding;
				sqlite3_bind_text(stmt, i + 1, call(s, to_cstring), -1, SQLITE_STATIC);
			} else if(type_equal(binding, "Integer")) {
				Integer * ii  = (Integer *) binding;
				sqlite3_bind_int(stmt, i + 1, ii->value);
			} else if(type_equal(binding, "Double")) {
				Double * d = (Double *) binding;
				sqlite3_bind_double(stmt, i + 1, d->value);
			}
			REFCDEC(binding);
		}
	}
	return stmt;
}

static bool exec_statement(ObjectPtr _this, DbPreparedStatement * _stmt, List * bindings) {
	sqlite3_stmt * stmt = bind_statement(_stmt, bindings);
	int rc = sqlite3_step(stmt);
	return rc == SQLITE_DONE;
}

static List * query_statement(ObjectPtr _this, DbPreparedStatement * _stmt, List * bindings) {
	sqlite3_stmt * stmt = bind_statement(_stmt, bindings);

	List * results = new(List);

	while(sqlite3_step(stmt) == SQLITE_ROW) {
		Map * result = new(Map);
		for(int i = 0; i < sqlite3_column_count(stmt); ++i) {
			String * column = new(String, sqlite3_column_name(stmt, i));
			Object * value = get_value(stmt, i);
			call(result, put, column, value);
			REFCDEC(column);
			REFCDEC(value);
		}
		call(results, add, result);
		REFCDEC(result);
	}

	return results;
}

static List * query(ObjectPtr _this, String * sql) {
	make_this(SQLiteConnection, _this);
	DbPreparedStatement * stmt = prepare_statement(this, sql);
	if(stmt == NULL) {
		return NULL;
	}
	List * results = query_statement(this, stmt, NULL);
	REFCDEC(stmt);
	return results;
}

static int last_error_code(ObjectPtr _this) {
	make_this(SQLiteConnection, _this);
	return sqlite3_errcode(this->_db);
}

static String * last_error_msg(ObjectPtr _this) {
	make_this(SQLiteConnection, _this);
	return new(String, sqlite3_errmsg(this->_db));
}

static long last_inserted_id(ObjectPtr _this) {
	make_this(SQLiteConnection, _this);
	return sqlite3_last_insert_rowid(this->_db);
}



SQLiteConnection * SQLiteConnection_new1(SQLiteConnection * this, sqlite3 *db) {
	super(DbConnection, SQLiteConnection);
	override(DbConnection, exec, exec);
	override(DbConnection, close, close);
	override(DbConnection, query, query);
	override(DbConnection, prepare_statement, prepare_statement);
	override(DbConnection, exec_statement, exec_statement);
	override(DbConnection, query_statement, query_statement);
	override(DbConnection, last_error_code, last_error_code);
	override(DbConnection, last_error_msg, last_error_msg);
	override(DbConnection, last_inserted_id, last_inserted_id);
	this->_db = db;
	return this;
}

void SQLiteConnection_delete(ObjectPtr _this) {
	super_delete(DbConnection, _this);
}

