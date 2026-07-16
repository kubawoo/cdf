#include "jsonobject.h"
#include <assert.h>
#include <math.h>


static void integer_to_json(void)
{
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_int_value");
    call(json, put_value, name, REFCTMP(new(Integer, 123)));
    assert(json != NULL);

    Object * value = call(json, get_value, name);
    assert(type_equal(value, "Integer"));
	Integer * int_value = (Integer *) value;
	assert((int_value->value) == (123));
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_int_value\":123}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void long_to_json(void)
{
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_long_value");
    call(json, put_value, name, REFCTMP(new(Long, 321)));
    assert(json != NULL);

    Object * value = call(json, get_value, name);
    assert(type_equal(value, "Long"));
	Long * long_value = (Long *) value;
	assert((long_value->value) == (321L));
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_long_value\":321}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void null_to_json(void)
{
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_null_value");
    call(json, put_value, name, NULL);
    assert(json != NULL);

    Object * value = call(json, get_value, name);
    assert(value == NULL);
    REFCDEC(name);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_null_value\":null}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void double_to_json(void)
{
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_double");
    call(json, put_value, name, REFCTMP(new(Double, 123.45)));
    assert(json != NULL);

    Object * value = call(json, get_value, name);
    assert(type_equal(value, "Double"));
	Double * double_value = (Double*) value;
	assert(fabs((double_value->value) - (123.45)) <= 0.000000001);
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_double\":123.4500}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void string_to_json(void)
{
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_string");
    call(json, put_value, name, REFCTMP(new(String, " this is my string value 123  {}!@#$%^&*()_ ")));
    assert(json != NULL);

    Object * value = call(json, get_value, name);
    assert(type_equal(value, "String"));
    String * string_value = (String*) value;
    assert(strcmp(call(string_value, to_cstring), " this is my string value 123  {}!@#$%^&*()_ ") == 0);
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_string\":\" this is my string value 123  {}!@#$%^&*()_ \"}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void boolean_to_json(void)
{
    JsonObject * json = new(JsonObject);
    String * name = new(String, "my_bool");
    call(json, put_value, name, REFCTMP(new(Boolean, true)));
    assert(json != NULL);

    Object * value = call(json, get_value, name);
    assert(type_equal(value, "Boolean"));
    Boolean * bool_value = (Boolean*) value;
	assert(bool_value->value);
	REFCDEC(name);
	REFCDEC(value);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_bool\":true}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void int_array_to_json(void)
{
    List * a = REFCTMP(new(List));
    call(a, add, REFCTMP(new(Integer, 5)));
    call(a, add, REFCTMP(new(Integer, 10)));
    call(a, add, REFCTMP(new(Integer, 15)));

    JsonObject * json = new(JsonObject);
    call(json, put_value, (String *) REFCTMP(new(String, "my_array")), a);
    assert(json != NULL);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_array\":[5,10,15]}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void string_array_to_json(void)
{
    List * a = REFCTMP(new(List));
    call(a, add, REFCTMP(new(String, "a")));
    call(a, add, REFCTMP(new(String, "b")));
    call(a, add, REFCTMP(new(String, "c")));

    JsonObject * json = new(JsonObject);
    call(json, put_value, (String *) REFCTMP(new(String, "my_array")), a);
    assert(json != NULL);

    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_array\":[\"a\",\"b\",\"c\"]}") == 0);
    REFCDEC(json_string);
    REFCDEC(json);
}

static void object_to_json(void)
{
	JsonObject * json_internal = new(JsonObject);
	call(json_internal, put_value, REFCTMP(new(String, "name")), REFCTMP(new(String, "John Doe")));
	call(json_internal, put_value, REFCTMP(new(String, "active")), REFCTMP(new(Boolean, false)));
	call(json_internal, put_value, REFCTMP(new(String, "age")), REFCTMP(new(Integer, 45)));


    JsonObject * json = new(JsonObject);
    call(json, put_value, REFCTMP(new(String, "my_object")), json_internal);

    assert(json != NULL);
    String * json_string = call(json, to_string);
    assert(strcmp(call(json_string, to_cstring), "{\"my_object\":{\"name\":\"John Doe\",\"active\":false,\"age\":45}}") == 0);
    REFCDEC(json_string);
    REFCDEC(json_internal);
    REFCDEC(json);
}

static void complex_object_to_json(void)
{
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
    assert(strcmp(call(json_string, to_cstring), "{\"name\":\"John Doe\",\"active\":false,\"age\":45,\"children\":[{\"name\":\"Alice\",\"age\":5},{\"name\":\"Bob\",\"age\":3}]}") == 0);
    REFCDEC(json_string);
    REFCDEC(john);
    REFCDEC(alice);
    REFCDEC(bob);
    REFCDEC(children);
}

static void get_missing_object(void)
{
	JsonObject * json = new(JsonObject);
	String * name = new(String, "no_such_value");
	Object * value = call(json, get_value, name);
	assert(value == NULL);

	REFCDEC(json);
	REFCDEC(name);
}

static void get_map(void)
{
	JsonObject * json = new(JsonObject);
	String * name = new(String, "my_int_value");
	call(json, put_value, name, REFCTMP(new(Integer, 123)));
	assert(json != NULL);

	Map * m = call(json, get_map);
	REFCDEC(json);

	Object * value = call(m, get, name);
	assert(type_equal(value, "Integer"));
	Integer * int_value = (Integer *) value;
	assert((int_value->value) == (123));
	REFCDEC(name);
	REFCDEC(value);
	REFCDEC(m);
	assert(m == NULL);
}
int main(void)
{
    integer_to_json();
    long_to_json();
    null_to_json();
    double_to_json();
    string_to_json();
    boolean_to_json();
    int_array_to_json();
    string_array_to_json();
    object_to_json();
    complex_object_to_json();
    get_missing_object();
    get_map();
    return 0;
}


