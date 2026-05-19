#include "test_framework.h"
#include "ooc.h"


void map_test(TEST_CASE_ARGUMENTS) {
    Map * map = new(Map);
    ASSERT_TRUE(type_equal(map, "Map"));
    ASSERT_EQUAL(call(map, get_length), 0);
    ASSERT_NOT_NULL(map->_keys);
    ASSERT_NOT_NULL(map->_values);

    String * key1 = new(String, "key1");
    String * key2 = new(String, "key2");
    String * key3 = new(String, "key3");

    call(map, put, REFCTMP(key1), REFCTMP(new(String, "abc")));
    call(map, put, REFCTMP(key2), REFCTMP(new(Integer, 5)));
    ASSERT_EQUAL(call(map, get_length), 2);

    ASSERT_NULL(call(map, get, (Object*)key3));

    String * v = call(map, get, (Object*)key1);
    ASSERT_STRINGS_EQUAL(call(v, to_cstring), "abc");
    REFCDEC(v);

    Integer * i = call(map, get, (Object *)key2);
    ASSERT_EQUAL(i->value, 5);
    REFCDEC(i);
    call(map, remove, (Object *)key2);

    ASSERT_EQUAL(call(map, get_length), 1);

    REFCDEC(key3);
    REFCDEC(map);
}

void map_to_string(TEST_CASE_ARGUMENTS) {
	Map * map = new(Map);

	String * key1 = new(String, "key1");
	String * key2 = new(String, "key2");
	String * key3 = new(String, "key3");

	call(map, put, REFCTMP(key1), REFCTMP(new(String, "abc")));
	call(map, put, REFCTMP(key2), REFCTMP(new(Integer, 5)));
	call(map, put, REFCTMP(key3), NULL);

	String * s = call(map, to_string);
	ASSERT_STRINGS_EQUAL(call(s, to_cstring), "{key1=abc; key2=5; key3=NULL}");

	REFCDEC(s);
	REFCDEC(map);
}

void map_update(TEST_CASE_ARGUMENTS) {
	Map * map = new(Map);
	String * key1 = new(String, "key1");
	call(map, put, key1, NULL);
	ASSERT_TRUE(call(map, contains_key, key1));
	ASSERT_TRUE(call(map, get, key1) == NULL);
	ASSERT_EQUAL(call(map, get_length), 1);

	call(map, put, key1, REFCTMP(new(String, "abc")));
	ASSERT_EQUAL(call(map, get_length), 1);

	String * abc = new(String, "abc");
	String * map_abc = call(map, get, key1);
	ASSERT_STRINGS_EQUAL(call(map_abc, to_cstring), call(abc, to_cstring));

	REFCDEC(map_abc);
	REFCDEC(abc);
	REFCDEC(key1);
	REFCDEC(map);
}

TEST_CASES_BEGIN
    TEST_CASE(map_test);
	TEST_CASE(map_to_string);
	TEST_CASE(map_update);
TEST_CASES_END
