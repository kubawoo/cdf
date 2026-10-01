

#include "eventparser.h"
#include "parserhandlers.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

typedef struct {
    inherits(JsonEventsHandler);
    void (*object_begin)(ObjectPtr, String *);
    void (*object_end)(ObjectPtr);
    void (*array_begin)(ObjectPtr, String *);
    void (*array_end)(ObjectPtr);
    void (*value)(ObjectPtr, String *, Object *);
    int array_begin_count;
    int array_end_count;
    int object_begin_count;
    int object_end_count;
    int value_count;
    int value_string_count;
    int value_integer_count;
    int value_real_count;
    int value_bool_count;
    int value_null_count;
} JsonCountingEventsHandler;

JsonCountingEventsHandler * JsonCountingEventsHandler_new(JsonCountingEventsHandler *);
void JsonCountingEventsHandler_delete(ObjectPtr);


static void testcase(void)
{
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "{ \"name\": \"John Doe\", \"age\": 45, \"is_active\": false, \"amount\": 123.43 }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));
    assert((handler->value_count) == (4));
    assert((handler->value_integer_count) == (1));
    assert((handler->value_bool_count) == (1));
    assert((handler->value_string_count) == (1));
    assert((handler->value_real_count) == (1));

    REFCDEC(parser);
}

static void nohandler(void)
{
    JsonEventsParser * parser = new(JsonEventsParser, NULL);
    String * json = new(String, "{ }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_NO_HANDLER));
    REFCDEC(parser);
}

static void null_input(void)
{
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser,(JsonEventsHandler *) handler);
    int ret = call(parser, parse, NULL);
    assert((ret) == (CJSON_PARSE_NO_INPUT));
    REFCDEC(parser);
}


static void invalid_json_tc1(void)
{
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "123{ }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_INVALID_JSON));
    REFCDEC(parser);
}

static void invalid_json_tc2(void)
{
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "{ } 32");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_INVALID_JSON));
    REFCDEC(parser);
}

static void build_object(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    const char * expected_json = "{\"name\":\"John Doe\",\"age\":45,\"is_active\":false,\"amount\":123.4300}";

    String * json = new(String, "{ \"name\": \"John Doe\", \"age\": 45, \"is_active\": false, \"amount\": 123.43 }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    String * object_string = call(handler->_object, to_string);
    assert(strcmp(call(object_string, to_cstring), expected_json) == 0);

    REFCDEC(object_string);
    REFCDEC(parser);
}


static void build_complex_object(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    const char * expected_json = "{\"name\":\"John Doe\",\"active\":false,\"age\":45,\"children\":[{\"name\":\"Alice\",\"age\":5,\"parent\":\"John Doe\"},{\"name\":\"Bob\",\"age\":3,\"parent\":\"John Doe\"}]}";

    String * json = new(String, "{\"name\": \"John Doe\",\n\"active\": false,\n\"age\": 45,\n\"children\": \n[\n{\"name\": \"Alice\",\n\"age\": 5,\"parent\": \"John Doe\"\n},\n{\"name\": \"Bob\",\n\"age\": 3,\n\"parent\": \"John Doe\"\n}\n]\n}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    printf("ret=%d\n", ret);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    String * object_string = call(handler->_object, to_string);
    assert(strcmp(call(object_string, to_cstring), expected_json) == 0);

    REFCDEC(object_string);
    REFCDEC(parser);
}

static void build_string_array(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "{\"strings\":[\"hello\",\"world\",\"foo\"]}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    String * name = new(String, "strings");
    Object * value = call(handler->_object, get_value, name);
    assert(value != NULL);
    assert(type_equal(value, "List"));
    List * list = (List *) value;
    assert(call(list, size) == (3));

    String * s0 = call(list, get, 0);
    assert(strcmp(call(s0, to_cstring), "hello") == 0);
    REFCDEC(s0);

    String * s1 = call(list, get, 1);
    assert(strcmp(call(s1, to_cstring), "world") == 0);
    REFCDEC(s1);

    String * s2 = call(list, get, 2);
    assert(strcmp(call(s2, to_cstring), "foo") == 0);
    REFCDEC(s2);

    REFCDEC(name);
    REFCDEC(value);
    REFCDEC(parser);
}

static void build_object_with_array_then_fields(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "{\"items\":[\"hello\",\"world\"],\"total\":2,\"active\":true}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    String * items_name = new(String, "items");
    Object * items_val = call(handler->_object, get_value, items_name);
    assert(items_val != NULL);
    assert(type_equal(items_val, "List"));
    List * items = (List *) items_val;
    assert(call(items, size) == (2));
    String * s0 = call(items, get, 0);
    assert(strcmp(call(s0, to_cstring), "hello") == 0);
    REFCDEC(s0);
    String * s1 = call(items, get, 1);
    assert(strcmp(call(s1, to_cstring), "world") == 0);
    REFCDEC(s1);
    REFCDEC(items_val);
    REFCDEC(items_name);

    String * total_name = new(String, "total");
    Object * total_val = call(handler->_object, get_value, total_name);
    assert(total_val != NULL);
    assert(type_equal(total_val, "Long"));
    Long * total = (Long *) total_val;
    assert((total->value) == (2L));
    REFCDEC(total_name);
    REFCDEC(total_val);

    String * active_name = new(String, "active");
    Object * active_val = call(handler->_object, get_value, active_name);
    assert(active_val != NULL);
    assert(type_equal(active_val, "Boolean"));
    Boolean * active = (Boolean *) active_val;
    assert(active->value);
    REFCDEC(active_name);
    REFCDEC(active_val);

    REFCDEC(parser);
}

static void parse_string_comma_value(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "{\"value\":\",\"}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    String * name = new(String, "value");
    Object * val = call(handler->_object, get_value, name);
    assert(val != NULL);
    assert(type_equal(val, "String"));
    String * s = (String *) val;
    assert(strcmp(call(s, to_cstring), ",") == 0);

    REFCDEC(name);
    REFCDEC(val);
    REFCDEC(parser);
}

/* Parses json and asserts the value at key, returning it for inspection. */
static String * value_of(const char * json_text, const char * key)
{
    JsonObjectBuilderEventsHandler * handler = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, json_text);
    InputStream * stream = new(StringInputStream, json);
    int ret = call(parser, parse, stream);
    REFCDEC(stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    String * name = new(String, key);
    Object * value = call(handler->_object, get_value, name);
    assert(value != NULL);
    assert(type_equal(value, "String"));
    String * text = (String *) value;
    REFCINC(text);
    REFCDEC(name);
    REFCDEC(parser);
    REFCDEC(handler);
    return text;
}

// An escaped quote is one character, not a quote that ends the string. Without
// this, a reply such as He said "OK" was parsed as He said \ followed by junk.
static void escaped_quote_is_decoded(void)
{
    String * v = value_of("{\"a\":\"say \\\"hi\\\"\"}", "a");
    assert(strcmp(call(v, to_cstring), "say \"hi\"") == 0);
    REFCDEC(v);
}

// The two-character escapes must become the control character they denote,
// rather than staying as a literal backslash followed by a letter.
static void short_escapes_are_decoded(void)
{
    String * nl = value_of("{\"a\":\"one\\ntwo\"}", "a");
    assert(strcmp(call(nl, to_cstring), "one\ntwo") == 0);
    REFCDEC(nl);

    String * tab = value_of("{\"a\":\"x\\ty\"}", "a");
    assert(strcmp(call(tab, to_cstring), "x\ty") == 0);
    REFCDEC(tab);

    String * slash = value_of("{\"a\":\"a\\/b\"}", "a");
    assert(strcmp(call(slash, to_cstring), "a/b") == 0);
    REFCDEC(slash);

    String * bs = value_of("{\"a\":\"a\\\\b\"}", "a");
    assert(strcmp(call(bs, to_cstring), "a\\b") == 0);
    REFCDEC(bs);
}

// A \uXXXX escape denotes one code point and must become its UTF-8 bytes, not
// six literal characters.
static void unicode_escape_is_decoded(void)
{
    String * v = value_of("{\"a\":\"\\u00e9\"}", "a");     // e-acute
    assert(strcmp(call(v, to_cstring), "\xc3\xa9") == 0);
    REFCDEC(v);

    String * tab = value_of("{\"a\":\"\\u0041\"}", "a");   // 'A'
    assert(strcmp(call(tab, to_cstring), "A") == 0);
    REFCDEC(tab);
}

// A brace or comma inside a quoted value is data. Reading it as structure would
// truncate the value and desynchronise the rest of the document.
static void structural_characters_inside_strings(void)
{
    String * brace = value_of("{\"a\":\"x}y\"}", "a");
    assert(strcmp(call(brace, to_cstring), "x}y") == 0);
    REFCDEC(brace);

    String * comma = value_of("{\"a\":\"x,y\",\"b\":2}", "a");
    assert(strcmp(call(comma, to_cstring), "x,y") == 0);
    REFCDEC(comma);
}

// An escaped brace is data for the same reason an unescaped one is.
static void escaped_structural_character(void)
{
    String * v = value_of("{\"a\":\"x\\}y\"}", "a");
    assert(strcmp(call(v, to_cstring), "x}y") == 0);
    REFCDEC(v);
}

// Escaping must not disturb the fields around the escaped value.
static void escape_does_not_break_neighbours(void)
{
    JsonObjectBuilderEventsHandler * handler = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "{\"content\":\"He said \\\"OK\\\".\",\"n\":7}");
    InputStream * stream = new(StringInputStream, json);
    assert(call(parser, parse, stream) == CJSON_PARSE_SUCCESS);

    String * ck = new(String, "content");
    Object * cv = call(handler->_object, get_value, ck);
    assert(type_equal(cv, "String"));
    assert(strcmp(call((String *)cv, to_cstring), "He said \"OK\".") == 0);

    String * nk = new(String, "n");
    Object * nv = call(handler->_object, get_value, nk);
    assert(type_equal(nv, "Long"));
    assert(((Long *)nv)->value == 7);

    REFCDEC(nk); REFCDEC(nv); REFCDEC(ck); REFCDEC(cv);
    REFCDEC(stream); REFCDEC(json); REFCDEC(parser); REFCDEC(handler);
}

// Text that arrives as a JSON string value inside an array takes a different
// path through the parser, so it needs covering separately.
static void escaped_string_in_array(void)
{
    JsonObjectBuilderEventsHandler * handler = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "{\"items\":[\"a\\\"b\",\"c,d\"]}");
    InputStream * stream = new(StringInputStream, json);
    assert(call(parser, parse, stream) == CJSON_PARSE_SUCCESS);

    String * k = new(String, "items");
    Object * items = call(handler->_object, get_value, k);
    assert(type_equal(items, "List"));
    List * list = (List *) items;
    assert(call(list, size) == 2);

    String * first = call(list, get, 0);
    String * second = call(list, get, 1);
    assert(strcmp(call((String *)first, to_cstring), "a\"b") == 0);
    assert(strcmp(call((String *)second, to_cstring), "c,d") == 0);

    REFCDEC(first); REFCDEC(second);
    REFCDEC(k); REFCDEC(items); REFCDEC(stream); REFCDEC(json);
    REFCDEC(parser); REFCDEC(handler);
}

// Multi-byte UTF-8 passes through a string value unchanged. A byte above 0x7F
// must not be mistaken for end-of-input, or the value is cut short.
static void raw_utf8_is_preserved(void)
{
    String * accented = value_of("{\"a\":\"caf\xc3\xa9\"}", "a");
    assert(strcmp(call(accented, to_cstring), "caf\xc3\xa9") == 0);
    REFCDEC(accented);

    String * cjk = value_of("{\"a\":\"\xe4\xbd\xa0\xe5\xa5\xbd\"}", "a");
    assert(strcmp(call(cjk, to_cstring), "\xe4\xbd\xa0\xe5\xa5\xbd") == 0);
    REFCDEC(cjk);
}

static void build_string_with_commas(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "{\"title\":\"Buy groceries\",\"description\":\"Milk, eggs, bread\",\"due_date\":\"2026-05-20\",\"assignee\":\"Alice\"}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    String * title_name = new(String, "title");
    Object * title_val = call(handler->_object, get_value, title_name);
    assert(title_val != NULL);
    assert(type_equal(title_val, "String"));
    String * title = (String *) title_val;
    assert(strcmp(call(title, to_cstring), "Buy groceries") == 0);
    REFCDEC(title_name);
    REFCDEC(title_val);

    String * desc_name = new(String, "description");
    Object * desc_val = call(handler->_object, get_value, desc_name);
    assert(desc_val != NULL);
    assert(type_equal(desc_val, "String"));
    String * desc = (String *) desc_val;
    assert(strcmp(call(desc, to_cstring), "Milk, eggs, bread") == 0);
    REFCDEC(desc_name);
    REFCDEC(desc_val);

    String * date_name = new(String, "due_date");
    Object * date_val = call(handler->_object, get_value, date_name);
    assert(date_val != NULL);
    assert(type_equal(date_val, "String"));
    String * date = (String *) date_val;
    assert(strcmp(call(date, to_cstring), "2026-05-20") == 0);
    REFCDEC(date_name);
    REFCDEC(date_val);

    String * ass_name = new(String, "assignee");
    Object * ass_val = call(handler->_object, get_value, ass_name);
    assert(ass_val != NULL);
    assert(type_equal(ass_val, "String"));
    String * ass = (String *) ass_val;
    assert(strcmp(call(ass, to_cstring), "Alice") == 0);
    REFCDEC(ass_name);
    REFCDEC(ass_val);

    REFCDEC(parser);
}
/* A document whose root is an array, as /v1/models returns. There is no parent
   object to hold it, so it lives only as the handler's own root. */
static void top_level_array_of_objects(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "[{\"id\":\"model-a\",\"object\":\"model\"},{\"id\":\"model-b\"}]");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    assert(handler->_object == NULL);

    List * list = call(handler, get_list);
    assert(list != NULL);
    assert(call(list, size) == (2));

    Object * first = call(list, get, 0);
    assert(type_equal(first, "JsonObject"));
    String * id = new(String, "id");
    Object * id_val = call((JsonObject *) first, get_value, id);
    assert(strcmp(call((String *) id_val, to_cstring), "model-a") == 0);
    REFCDEC(id_val);
    REFCDEC(id);
    REFCDEC(first);

    Object * second = call(list, get, 1);
    assert(type_equal(second, "JsonObject"));
    String * id2 = new(String, "id");
    Object * id_val2 = call((JsonObject *) second, get_value, id2);
    assert(strcmp(call((String *) id_val2, to_cstring), "model-b") == 0);
    REFCDEC(id_val2);
    REFCDEC(id2);
    REFCDEC(second);

    REFCDEC(list);
    REFCDEC(parser);
}

/* get_object must stay NULL for an array document, and the list must still be
   readable after it is handed back out. */
static void top_level_array_of_strings(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "[\"alpha\",\"beta\",\"gamma\"]");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    assert(call(handler, get_object) == NULL);

    List * list = call(handler, get_list);
    assert(list != NULL);
    assert(call(list, size) == (3));
    String * s1 = call(list, get, 1);
    assert(strcmp(call(s1, to_cstring), "beta") == 0);
    REFCDEC(s1);
    REFCDEC(list);

    REFCDEC(parser);
}

static void empty_top_level_array(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "[]");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    List * list = call(handler, get_list);
    assert(list != NULL);
    assert(call(list, size) == (0));
    REFCDEC(list);
    REFCDEC(parser);
}

/* An object document has no root array, so get_list must not invent one. */
static void top_level_object_has_no_root_list(void)
{
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "{\"id\":\"only\"}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    assert((ret) == (CJSON_PARSE_SUCCESS));

    assert(call(handler, get_list) == NULL);
    assert(call(handler, get_object) != NULL);
    REFCDEC(parser);
}

int main(void)
{
    testcase();
    nohandler();
    null_input();
    invalid_json_tc1();
    invalid_json_tc2();
    build_object();
    build_complex_object();
    build_string_array();
    build_object_with_array_then_fields();
    build_string_with_commas();
    parse_string_comma_value();
    escaped_quote_is_decoded();
    short_escapes_are_decoded();
    unicode_escape_is_decoded();
    structural_characters_inside_strings();
    escaped_structural_character();
    escape_does_not_break_neighbours();
    escaped_string_in_array();
    raw_utf8_is_preserved();
    top_level_array_of_objects();
    top_level_array_of_strings();
    empty_top_level_array();
    top_level_object_has_no_root_list();
    return 0;
}



void _ceh_array_begin(ObjectPtr _this, String * name) {
    make_this(JsonCountingEventsHandler, _this);
    this->array_begin_count++;
    const char * name_cstring = name == NULL ? "null" : call(name, to_cstring);
    printf("array_begin %s\n", name_cstring);
}

void _ceh_array_end(ObjectPtr _this) {
    make_this(JsonCountingEventsHandler, _this);
    this->array_end_count++;
    printf("array_end\n");
}

void _ceh_object_begin(ObjectPtr _this, String * name) {
    make_this(JsonCountingEventsHandler, _this);
    this->object_begin_count++;
    const char * name_cstring = name == NULL ? "null" : call(name, to_cstring);
    printf("object_begin %s\n", name_cstring);
}

void _ceh_object_end(ObjectPtr _this) {
    make_this(JsonCountingEventsHandler, _this);
    this->object_end_count++;
    printf("object_end\n");
}

void _ceh_value(ObjectPtr _this, String * name, Object * value) {
    make_this(JsonCountingEventsHandler, _this);
    const char * name_cstring = name == NULL ? "null" : call(name, to_cstring);
    String * value_string = value == NULL ? NULL : call(value, to_string);
    const char * value_cstring = value_string == NULL ? "null" : call(value_string, to_cstring);
    this->value_count++;
    if(value == NULL) {
        this->value_null_count++;
        printf("NULL (%s=%s)\n", name_cstring, value_cstring);
    } else if(type_equal(value, "Integer") || type_equal(value, "Long")) {
        this->value_integer_count++;
        printf("INTEGER (%s=%s)\n", name_cstring, value_cstring);
    } else if(type_equal(value, "Double")) {
        this->value_real_count++;
        printf("REAL (%s=%s)\n", name_cstring, value_cstring);
    } else if(type_equal(value, "Boolean")) {
        this->value_bool_count++;
        printf("BOOLEAN (%s=%s)\n", name_cstring, value_cstring);
    } else if(type_equal(value, "String")) {
        printf("STRING (%s=%s)\n", name_cstring, value_cstring);
        this->value_string_count++;
    }
    REFCDEC(value_string);
}

void JsonCountingEventsHandler_delete(ObjectPtr _this) {
    //empty destructor
    super_delete(JsonEventsHandler, _this);
}

JsonCountingEventsHandler * JsonCountingEventsHandler_new(JsonCountingEventsHandler * this) {
    super(JsonEventsHandler, JsonCountingEventsHandler);
    override(JsonEventsHandler, value, _ceh_value);
    override(JsonEventsHandler, array_begin, _ceh_array_begin);
    override(JsonEventsHandler, array_end, _ceh_array_end);
    override(JsonEventsHandler, object_begin, _ceh_object_begin);
    override(JsonEventsHandler, object_end, _ceh_object_end);
    this->array_begin_count = 0;
    this->array_end_count = 0;
    this->object_begin_count = 0;
    this->object_end_count = 0;
    this->value_count = 0;
    this->value_string_count = 0;
    this->value_integer_count = 0;
    this->value_real_count = 0;
    this->value_bool_count = 0;
    this->value_null_count = 0;
    return this;
}


