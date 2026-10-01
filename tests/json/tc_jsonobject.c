#include "jsonobject.h"
#include "eventparser.h"
#include "parserhandlers.h"
#include "stringinputstream.h"
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
/* Serializes a one-entry object whose value is the given text, returning the
   document. Caller frees the result. */
static String * json_with_value(const char * value)
{
    JsonObject * json = new(JsonObject);
    call(json, put_value, REFCTMP(new(String, "v")), REFCTMP(new(String, value)));
    String * text = call(json, to_string);
    REFCDEC(json);
    return text;
}

// A value containing a quote must be escaped, otherwise the document is invalid
// JSON and the receiver rejects it.
static void quote_in_value_is_escaped(void)
{
    String * expected = json_with_value("he said \"hi\"");
    assert(strcmp(call(expected, to_cstring), "{\"v\":\"he said \\\"hi\\\"\"}") == 0);
    REFCDEC(expected);
}

// Control characters have no place raw in JSON and must use their escapes.
static void control_characters_are_escaped(void)
{
    String * nl = json_with_value("a\nb");
    assert(strcmp(call(nl, to_cstring), "{\"v\":\"a\\nb\"}") == 0);
    REFCDEC(nl);

    String * tab = json_with_value("a\tb");
    assert(strcmp(call(tab, to_cstring), "{\"v\":\"a\\tb\"}") == 0);
    REFCDEC(tab);

    String * cr = json_with_value("a\rb");
    assert(strcmp(call(cr, to_cstring), "{\"v\":\"a\\rb\"}") == 0);
    REFCDEC(cr);

/* Backspace and formfeed are C escapes of their own, so the values are built
   from explicit bytes: the encoder must emit the two-character JSON escape. */
    JsonObject * bsj = new(JsonObject);
    String * bs = new(String);
    call(bs, append_cstring, "a");
    call(bs, append_char, (char) 0x08);
    call(bs, append_char, 'b');
    call(bsj, put_value, REFCTMP(new(String, "v")), bs);
    String * bst = call(bsj, to_string);
    assert(strcmp(call(bst, to_cstring), "{\"v\":\"a\\bb\"}") == 0);
    REFCDEC(bst); REFCDEC(bsj); REFCDEC(bs);

    JsonObject * ffj = new(JsonObject);
    String * ff = new(String);
    call(ff, append_cstring, "a");
    call(ff, append_char, (char) 0x0C);
    call(ff, append_char, 'b');
    call(ffj, put_value, REFCTMP(new(String, "v")), ff);
    String * fft = call(ffj, to_string);
    assert(strcmp(call(fft, to_cstring), "{\"v\":\"a\\fb\"}") == 0);
    REFCDEC(fft); REFCDEC(ffj); REFCDEC(ff);
}

// A backslash must be doubled, or the document would claim an escape that the
// value never contained.
static void backslash_in_value_is_escaped(void)
{
    String * expected = json_with_value("a\\b");
    assert(strcmp(call(expected, to_cstring), "{\"v\":\"a\\\\b\"}") == 0);
    REFCDEC(expected);
}

// Control characters with no two-character form go out as \u00XX.
static void other_control_characters_use_unicode_escape(void)
{
    JsonObject * json = new(JsonObject);
    String * value = new(String);
    call(value, append_char, 'a');
    call(value, append_char, (char) 0x01);
    call(value, append_char, 'b');
    call(json, put_value, REFCTMP(new(String, "v")), value);
    String * text = call(json, to_string);
    assert(strcmp(call(text, to_cstring), "{\"v\":\"a\\u0001b\"}") == 0);
    REFCDEC(text); REFCDEC(value); REFCDEC(json);
}

// Text outside the escaped set, including UTF-8, must survive untouched.
static void plain_and_utf8_pass_through(void)
{
    JsonObject * plain = new(JsonObject);
    call(plain, put_value, REFCTMP(new(String, "v")), REFCTMP(new(String, "hello world")));
    String * p = call(plain, to_string);
    assert(strcmp(call(p, to_cstring), "{\"v\":\"hello world\"}") == 0);
    REFCDEC(p); REFCDEC(plain);

    JsonObject * utf8 = new(JsonObject);
    call(utf8, put_value, REFCTMP(new(String, "v")), REFCTMP(new(String, "caf\xc3\xa9")));
    String * u = call(utf8, to_string);
    assert(strcmp(call(u, to_cstring), "{\"v\":\"caf\xc3\xa9\"}") == 0);
    REFCDEC(u); REFCDEC(utf8);
}

// Escaping a value must leave the keys and the surrounding structure intact.
static void escaped_value_keeps_document_shape(void)
{
    JsonObject * json = new(JsonObject);
    call(json, put_value, REFCTMP(new(String, "content")), REFCTMP(new(String, "a\"b")));
    call(json, put_value, REFCTMP(new(String, "n")), REFCTMP(new(Long, 7L)));
    String * text = call(json, to_string);
    const char * s = call(text, to_cstring);
    assert(strstr(s, "{\"content\":\"a\\\"b\",") != NULL);
    assert(strstr(s, "\"n\":7}") != NULL);
    REFCDEC(text);
    REFCDEC(json);
}

// Whatever to_string emits must parse back to the value that went in.
static void escaped_value_round_trips(void)
{
    const char * originals[] = { "he said \"hi\"", "a\nb", "a\\b", "a\tb", "caf\xc3\xa9" };
    for(unsigned i = 0; i < sizeof originals / sizeof originals[0]; ++i) {
        JsonObject * json = new(JsonObject);
        call(json, put_value, REFCTMP(new(String, "v")), REFCTMP(new(String, originals[i])));
        String * text = call(json, to_string);
        REFCDEC(json);

        StringInputStream * stream = new(StringInputStream, text);
        JsonObjectBuilderEventsHandler * handler = new(JsonObjectBuilderEventsHandler);
        JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
        assert(call(parser, parse, (InputStream *) stream) == CJSON_PARSE_SUCCESS);

        String * key = new(String, "v");
        Object * value = call(handler->_object, get_value, key);
        assert(value != NULL);
        assert(strcmp(call((String *)value, to_cstring), originals[i]) == 0);

        REFCDEC(key); REFCDEC(value);
        REFCDEC(parser); REFCDEC(handler); REFCDEC(stream); REFCDEC(text);
    }
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
    quote_in_value_is_escaped();
    control_characters_are_escaped();
    backslash_in_value_is_escaped();
    other_control_characters_use_unicode_escape();
    plain_and_utf8_pass_through();
    escaped_value_keeps_document_shape();
    escaped_value_round_trips();
    return 0;
}


