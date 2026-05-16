#include "test_framework.h"
#include "jsonobject.h"

#include <ooc.h>

void integer_to_json(TEST_CASE_ARGUMENTS) {
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_int_value");
    call(json, put_value, name, REFCTMP(new(Integer, 123)));
    ASSERT_NOT_NULL(json);

    Object * value = call(json, get_value, name);
    ASSERT_TRUE(type_equal(value, "Integer"));
	Integer * int_value = (Integer *) value;
	ASSERT_EQUAL(int_value->value, 123);
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_int_value\":123}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void long_to_json(TEST_CASE_ARGUMENTS) {
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_long_value");
    call(json, put_value, name, REFCTMP(new(Long, 321)));
    ASSERT_NOT_NULL(json);

    Object * value = call(json, get_value, name);
    ASSERT_TRUE(type_equal(value, "Long"));
	Long * long_value = (Long *) value;
	ASSERT_EQUAL(long_value->value, 321L);
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_long_value\":321}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void null_to_json(TEST_CASE_ARGUMENTS) {
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_null_value");
    call(json, put_value, name, NULL);
    ASSERT_NOT_NULL(json);

    Object * value = call(json, get_value, name);
    ASSERT_NULL(value);
    REFCDEC(name);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_null_value\":null}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void double_to_json(TEST_CASE_ARGUMENTS) {
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_double");
    call(json, put_value, name, REFCTMP(new(Double, 123.45)));
    ASSERT_NOT_NULL(json);

    Object * value = call(json, get_value, name);
    ASSERT_TRUE(type_equal(value, "Double"));
	Double * double_value = (Double*) value;
	ASSERT_ALMOST_EQUAL(double_value->value, 123.45);
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_double\":123.4500}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void string_to_json(TEST_CASE_ARGUMENTS) {
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_string");
    call(json, put_value, name, REFCTMP(new(String, " this is my string value 123  {}!@#$%^&*()_ ")));
    ASSERT_NOT_NULL(json);

    Object * value = call(json, get_value, name);
    ASSERT_TRUE(type_equal(value, "String"));
    String * string_value = (String*) value;
    ASSERT_STRINGS_EQUAL(call(string_value, to_cstring), " this is my string value 123  {}!@#$%^&*()_ ");
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_string\":\" this is my string value 123  {}!@#$%^&*()_ \"}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void boolean_to_json(TEST_CASE_ARGUMENTS) {
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_bool");
    call(json, put_value, name, REFCTMP(new(Boolean, true)));
    ASSERT_NOT_NULL(json);

    Object * value = call(json, get_value, name);
    ASSERT_TRUE(type_equal(value, "Boolean"));
    Boolean * bool_value = (Boolean*) value;
	ASSERT_TRUE(bool_value->value);
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_bool\":true}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void int_array_to_json(TEST_CASE_ARGUMENTS) {
    List * a = REFCTMP(new(List));
    call(a, add, REFCTMP(new(Integer, 5)));
    call(a, add, REFCTMP(new(Integer, 10)));
    call(a, add, REFCTMP(new(Integer, 15)));

    JsonObject * json = new(JsonObject);
    call(json, put_value, (String *) REFCTMP(new(String, "my_array")), a);
    ASSERT_NOT_NULL(json);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_array\":[5,10,15]}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void string_array_to_json(TEST_CASE_ARGUMENTS) {
    List * a = REFCTMP(new(List));
    call(a, add, REFCTMP(new(String, "a")));
    call(a, add, REFCTMP(new(String, "b")));
    call(a, add, REFCTMP(new(String, "c")));

    JsonObject * json = new(JsonObject);
    call(json, put_value, (String *) REFCTMP(new(String, "my_array")), a);
    ASSERT_NOT_NULL(json);

    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_array\":[\"a\",\"b\",\"c\"]}");
    REFCDEC(json_string);
    REFCDEC(json);
}

void object_to_json(TEST_CASE_ARGUMENTS) {
	JsonObject * json_internal = new(JsonObject);
	call(json_internal, put_value, REFCTMP(new(String, "name")), REFCTMP(new(String, "John Doe")));
	call(json_internal, put_value, REFCTMP(new(String, "active")), REFCTMP(new(Boolean, false)));
	call(json_internal, put_value, REFCTMP(new(String, "age")), REFCTMP(new(Integer, 45)));


    JsonObject * json = new(JsonObject);
    call(json, put_value, REFCTMP(new(String, "my_object")), json_internal);

    ASSERT_NOT_NULL(json);
    String * json_string = call(json, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring), "{\"my_object\":{\"name\":\"John Doe\",\"active\":false,\"age\":45}}");
    REFCDEC(json_string);
    REFCDEC(json_internal);
    REFCDEC(json);
}

void complex_object_to_json(TEST_CASE_ARGUMENTS) {
    JsonObject * john = new(JsonObject);
    call(john, put_value, REFCTMP(new(String, "name")), REFCTMP(new(String, "John Doe")));
    call(john, put_value, REFCTMP(new(String, "active")), REFCTMP(new(Boolean, false)));
    call(john, put_value, REFCTMP(new(String, "age")), REFCTMP(new(Integer, 45)));

    List * children = new(List);
    JsonObject * alice = new(JsonObject);
    call(alice, put_value, REFCTMP(new(String, "name")), REFCTMP(new(String, "Alice")));
    call(alice, put_value, REFCTMP(new(String, "age")), REFCTMP(new(Integer, 5)));

    JsonObject * bob = new(JsonObject);
    call(bob, put_value, REFCTMP(new(String, "name")), REFCTMP(new(String, "Bob")));
    call(bob, put_value, REFCTMP(new(String, "age")), REFCTMP(new(Integer, 3)));

    call(children, add, alice);
    call(children, add, bob);
    call(john, put_value, REFCTMP(new(String, "children")), children);

    String * json_string = call(john, to_string);
    ASSERT_STRINGS_EQUAL(call(json_string, to_cstring),
     "{\"name\":\"John Doe\",\"active\":false,\"age\":45,\"children\":[{\"name\":\"Alice\",\"age\":5},{\"name\":\"Bob\",\"age\":3}]}");
    REFCDEC(json_string);
    REFCDEC(john);
    REFCDEC(alice);
    REFCDEC(bob);
    REFCDEC(children);
}

void get_missing_object(TEST_CASE_ARGUMENTS) {
	JsonObject * json = new(JsonObject);
	String * name = new(String, "no_such_value");
	Object * value = call(json, get_value, name);
	ASSERT_NULL(value);

	REFCDEC(json);
	REFCDEC(name);
}

void get_map(TEST_CASE_ARGUMENTS) {
	JsonObject * json = new(JsonObject);
	String * name = new(String, "my_int_value");
	call(json, put_value, name, REFCTMP(new(Integer, 123)));
	ASSERT_NOT_NULL(json);

	Map * m = call(json, get_map);
	REFCDEC(json);

	Object * value = call(m, get, name);
	ASSERT_TRUE(type_equal(value, "Integer"));
	Integer * int_value = (Integer *) value;
	ASSERT_EQUAL(int_value->value, 123);
	REFCDEC(name);
	REFCDEC(value);
	REFCDEC(m);
	ASSERT_NULL(m);
}

TEST_CASES_BEGIN
    TEST_CASE(integer_to_json);
    TEST_CASE(long_to_json);
    TEST_CASE(null_to_json);
    TEST_CASE(double_to_json);
    TEST_CASE(string_to_json);
    TEST_CASE(boolean_to_json);
    TEST_CASE(int_array_to_json);
    TEST_CASE(string_array_to_json);
    TEST_CASE(object_to_json);
    TEST_CASE(complex_object_to_json);
    TEST_CASE(get_missing_object);
    TEST_CASE(get_map);
TEST_CASES_END

