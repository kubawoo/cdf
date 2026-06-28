#include "database.h"
#include "connection.h"
#include "prepared_statement.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void database_lifecycle(void)
{
    Database * db = new(Database);
    assert(db != NULL);
    assert(type_equal(db, "Database"));
    String * s = call(db, to_string);
    assert(strcmp(call(s, to_cstring), "Database of unknown type") == 0);
    REFCDEC(s);
    REFCDEC(db);
}

static void connection_lifecycle(void)
{
    DbConnection * conn = new(DbConnection);
    assert(conn != NULL);
    assert(type_equal(conn, "DbConnection"));
    String * sql = new(String, "SELECT 1");
    bool ret = call(conn, exec, sql);
    assert(!(ret));
    List * results = call(conn, query, sql);
    assert(results == NULL);
    REFCDEC(conn);
    REFCDEC(sql);
}

static void prepared_statement_lifecycle(void)
{
    DbPreparedStatement * stmt = new(DbPreparedStatement);
    assert(stmt != NULL);
    assert(type_equal(stmt, "DbPreparedStatement"));
    REFCDEC(stmt);
}
int main(void)
{
    database_lifecycle();
    connection_lifecycle();
    prepared_statement_lifecycle();
    return 0;
}

