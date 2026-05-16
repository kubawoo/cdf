#include "sqlite_database.h"
#include "console.h"
#include "test_framework.h"

void db_test(TEST_CASE_ARGUMENTS) {
	Database * db = new(SQLiteDatabase);
	Console * c = new(Console);
	call(c, print_object, db);


	DbConnection * conn = call(db, open);
	ASSERT_NOT_NULL(conn);
	String * sql = new(String, "DROP TABLE IF EXISTS Cars;"
			"CREATE TABLE Cars(Id INT, Name TEXT, Price REAL);"
			"INSERT INTO Cars VALUES(1, 'Audi', 52642.99);"
			"INSERT INTO Cars VALUES(2, 'Mercedes', 57127.50);"
			"INSERT INTO Cars VALUES(3, 'Skoda', 9000.00);"
			"INSERT INTO Cars VALUES(4, 'Volvo', 29000.40);"
			"INSERT INTO Cars VALUES(5, 'Bentley', 350000.50);"
			"INSERT INTO Cars VALUES(6, 'Citroen', 21000.60);"
			"INSERT INTO Cars VALUES(7, 'Hummer', 41400.70);"
			"INSERT INTO Cars VALUES(8, 'Volkswagen', 21600.80);"
	);

	double start = tick_ms();
	bool ret = call(conn, exec, sql);
	REFCDEC(sql);
	ASSERT_TRUE(ret);
	fprintf(stderr, "\nCreating tables and inserting data took %.3lf ms\n", tick_ms() - start);

	sql = new(String, "SELECT * FROM Cars;");
	List * results = call(conn, query, sql);
	REFCDEC(sql);
	call(c, print_object, results);
	ASSERT_EQUAL(results->length, 8);
	REFCDEC(results);

	sql = new(String, "SELECT * FROM Cars WHERE Price > ?");
	DbPreparedStatement * stmt = call(conn, prepare_statement, sql);
	REFCDEC(sql);
	List * bindings = new(List);
	call(bindings, add, REFCTMP(new(Double, 30000.00)));

	results = call(conn, query_statement, stmt, bindings);

	REFCDEC(bindings);
	call(c, print_object, results);
	ASSERT_EQUAL(results->length, 4);
	REFCDEC(results);

	bindings = new(List);
	call(bindings, add, REFCTMP(new(Double, 50000.00)));

	results = call(conn, query_statement, stmt, bindings);

	call(c, print_object, results);
	ASSERT_EQUAL(results->length, 3);
	REFCDEC(results);

	sql = new(String, "INSERT INTO Cars VALUES (?, ?, ?);");
	DbPreparedStatement * insert_stmt = call(conn, prepare_statement, sql);
	REFCDEC(sql);

	List * insert_bindings = new(List);
	call(insert_bindings, add, REFCTMP(new(Integer, 9)));
	call(insert_bindings, add, REFCTMP(new(String, "Honda")));
	call(insert_bindings, add, REFCTMP(new(Double, 51000.00)));

	bool inserted = call(conn, exec_statement, insert_stmt, insert_bindings);
	ASSERT_TRUE(inserted);
	REFCDEC(insert_bindings);

	insert_bindings = new(List);
	call(insert_bindings, add, REFCTMP(new(Integer, 10)));
	call(insert_bindings, add, REFCTMP(new(String, "Mazda")));
	call(insert_bindings, add, REFCTMP(new(Double, 52000.00)));
	inserted = call(conn, exec_statement, insert_stmt, insert_bindings);
	ASSERT_TRUE(inserted);
	REFCDEC(insert_bindings);
	REFCDEC(insert_stmt);


	results = call(conn, query_statement, stmt, bindings);

	call(c, print_object, results);
	ASSERT_EQUAL(results->length, 5);
	REFCDEC(results);


	REFCDEC(bindings);
	REFCDEC(stmt);
	REFCDEC(conn);
	REFCDEC(db);
	REFCDEC(c);
}


TEST_CASES_BEGIN
TEST_CASE(db_test);
TEST_CASES_END

