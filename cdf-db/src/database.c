#include "database.h"

static String * to_string(ObjectPtr _this) {
	return new(String, "Database of unknown type");
}

static DbConnection * open_connection(ObjectPtr _this) {
	return NULL;
}

Database * Database_new(Database * this) {
    super(Object, Database);
    override(Object, to_string, to_string);
    this->open = open_connection;
    return this;
}

void Database_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}


