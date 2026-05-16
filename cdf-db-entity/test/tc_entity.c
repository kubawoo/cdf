#include "../samples/customer.h"
#include "test_framework.h"
#include "console.h"

void entity_test(TEST_CASE_ARGUMENTS) {
	Console * console = new(Console);
	Entity * customer = new(Customer, REFCTMP(new(String, "John")), REFCTMP(new(String, "Doe")), REFCTMP(new(Integer, 45)));
	customer->id = new(Long, 1L);

	Map * customer_map = call(customer, to_map, NULL);
	call(console, print_object, customer_map);
	Customer * customer2 = new(Customer);
	call((Entity*)customer2, from_map, customer_map, NULL);

	ASSERT_EQUAL(((Entity*)customer2)->id->value, 1L);
	ASSERT_STRINGS_EQUAL(call(customer2->first_name, to_cstring), "John");
	ASSERT_STRINGS_EQUAL(call(customer2->last_name, to_cstring), "Doe");
	ASSERT_EQUAL(customer2->age->value, 45);
	String * table = call(customer, table_name);
	ASSERT_STRINGS_EQUAL(call(table, to_cstring), "Customer");

	REFCDEC(table);
	REFCDEC(customer2);
	REFCDEC(customer_map);


	String * json = call(customer, to_json, NULL);

	call(console, print_object, json);
	customer2 = new(Customer);
	call((Entity*)customer2, from_json, json, NULL);

	ASSERT_EQUAL(((Entity*)customer2)->id->value, 1L);
	ASSERT_STRINGS_EQUAL(call(customer2->first_name, to_cstring), "John");
	ASSERT_STRINGS_EQUAL(call(customer2->last_name, to_cstring), "Doe");
	ASSERT_EQUAL(customer2->age->value, 45);
	REFCDEC(customer2);

	REFCDEC(json);
	REFCDEC(console);

	REFCDEC(customer);
}

void entity_reflection_test(TEST_CASE_ARGUMENTS) {
	Console * console = new(Console);
	Entity * customer1 = new(Customer, REFCTMP(new(String, "John")), REFCTMP(new(String, "Doe")), REFCTMP(new(Integer, 23)));
	Entity * customer2 = new(Customer, REFCTMP(new(String, "Jack")), REFCTMP(new(String, "Zoe")), REFCTMP(new(Integer, 34)));
	customer2->id = new(Long, 1L);

	List * fields = call(customer1, fields, true);

	Map * map = call(customer1, to_map, fields);
	call(console, print_object, map);

	Map * map2 = call(customer2, to_map, fields);
	call(console, print_object, map2);

	Entity * customer3 = new(Customer);
	call(customer3, from_map, map, fields);

	ASSERT_EQUAL(customer3->id, NULL);
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer3)->first_name, to_cstring), "John");
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer3)->last_name, to_cstring), "Doe");
	ASSERT_EQUAL(((Customer*)customer3)->age->value, 23);

	call(customer3, from_map, map2, fields);
	ASSERT_EQUAL(customer3->id->value, 1L);
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer3)->first_name, to_cstring), "Jack");
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer3)->last_name, to_cstring), "Zoe");
	ASSERT_EQUAL(((Customer*)customer3)->age->value, 34);

	call(customer3, from_map, map, fields);

	ASSERT_EQUAL(customer3->id, NULL);
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer3)->first_name, to_cstring), "John");
	ASSERT_STRINGS_EQUAL(call(((Customer*)customer3)->last_name, to_cstring), "Doe");
	ASSERT_EQUAL(((Customer*)customer3)->age->value, 23);



	REFCDEC(console);
	REFCDEC(customer1);
	REFCDEC(customer2);
	REFCDEC(customer3);
	REFCDEC(fields);
	REFCDEC(map);
	REFCDEC(map2);


}


TEST_CASES_BEGIN
	TEST_CASE(entity_test);
	TEST_CASE(entity_reflection_test);
TEST_CASES_END

