#include "connection.h"


static bool exec(ObjectPtr _this, String * sql) {
	return false;
}

static List * query(ObjectPtr _this, String * sql) {
	return NULL;
}

static void close(ObjectPtr _this) {
}

DbConnection * DbConnection_new(DbConnection * this) {
	super(Object, DbConnection);
	this->exec = exec;
	this->close = close;
	this->query = query;
	this->prepare_statement = NULL;
	this->exec_statement = NULL;
	this->query_statement = NULL;
	this->last_error_code = NULL;
	this->last_error_msg = NULL;
	this->last_inserted_id = NULL;
	return this;
}

void DbConnection_delete(ObjectPtr _this) {
	make_this(DbConnection, _this);
	call(this, close);
	super_delete(Object, _this);
}

