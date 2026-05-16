#include "prepared_statement.h"

DbPreparedStatement * DbPreparedStatement_new(DbPreparedStatement * this) {
	super(Object, DbPreparedStatement);
	return this;
}

void DbPreparedStatement_delete(ObjectPtr _this) {
	super_delete(Object, _this);
}

