#include "test_framework.h"
#include "database.h"
#include "connection.h"
#include "prepared_statement.h"

void database_lifecycle(TEST_CASE_ARGUMENTS) {
    Database * db = new(Database);
    ASSERT_NOT_NULL(db);
    ASSERT_TRUE(type_equal(db, "Database"));
    String * s = call(db, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "Database of unknown type");
    REFCDEC(s);
    REFCDEC(db);
}

void connection_lifecycle(TEST_CASE_ARGUMENTS) {
    DbConnection * conn = new(DbConnection);
    ASSERT_NOT_NULL(conn);
    ASSERT_TRUE(type_equal(conn, "DbConnection"));
    String * sql = new(String, "SELECT 1");
    bool ret = call(conn, exec, sql);
    ASSERT_FALSE(ret);
    List * results = call(conn, query, sql);
    ASSERT_NULL(results);
    REFCDEC(conn);
    REFCDEC(sql);
}

void prepared_statement_lifecycle(TEST_CASE_ARGUMENTS) {
    DbPreparedStatement * stmt = new(DbPreparedStatement);
    ASSERT_NOT_NULL(stmt);
    ASSERT_TRUE(type_equal(stmt, "DbPreparedStatement"));
    REFCDEC(stmt);
}

TEST_CASES_BEGIN
    TEST_CASE(database_lifecycle);
    TEST_CASE(connection_lifecycle);
    TEST_CASE(prepared_statement_lifecycle);
TEST_CASES_END
