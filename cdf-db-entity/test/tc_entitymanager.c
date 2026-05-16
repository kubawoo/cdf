#include "../samples/customer.h"
#include "test_framework.h"
#include "console.h"
#include "entitymanager.h"
#include "sqlite_database.h"

void entitymanager_test(TEST_CASE_ARGUMENTS) {
	Entity * customer = new(Customer, REFCTMP(new(String, "John")), REFCTMP(new(String, "Doe")), REFCTMP(new(Integer, 55)));
	Database * db = new(SQLiteDatabase);

	EntityManager * em = new(EntityManager, db);
	REFCDEC(db);
	call(em, save, customer);

	ASSERT_EQUAL(customer->id->value, 1L);
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer)->first_name, to_cstring), "John");
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer)->last_name, to_cstring), "Doe");
	ASSERT_EQUAL(((Customer*)customer)->age->value, 55);

	Entity * loaded_customer = new(Customer);
	loaded_customer->id = REFCTMP(new(Long, 1L));
    call(em, load, loaded_customer);

	ASSERT_EQUAL(loaded_customer->id->value, 1L);
	ASSERT_NOT_NULL(((Customer*)loaded_customer)->first_name);
	ASSERT_STRINGS_EQUAL(call(((Customer*)loaded_customer)->first_name, to_cstring), "John");
	ASSERT_NOT_NULL(((Customer*)loaded_customer)->last_name);
	ASSERT_STRINGS_EQUAL(call(((Customer*)loaded_customer)->last_name, to_cstring), "Doe");
	ASSERT_NOT_NULL(((Customer*)loaded_customer)->age);
	ASSERT_EQUAL(((Customer*)loaded_customer)->age->value, 55);

    call(em, remove, customer);

    call(em, load, loaded_customer);
	ASSERT_EQUAL(loaded_customer->id->value, -1L);

	REFCDEC(em);
	REFCDEC(customer);
	REFCDEC(loaded_customer);
}


TEST_CASES_BEGIN
	TEST_CASE(entitymanager_test);
TEST_CASES_END

