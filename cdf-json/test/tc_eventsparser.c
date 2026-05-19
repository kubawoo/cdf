
#include "test_framework.h"

#include "eventparser.h"
#include "parserhandlers.h"

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


void testcase(TEST_CASE_ARGUMENTS) {
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "{ \"name\": \"John Doe\", \"age\": 45, \"is_active\": false, \"amount\": 123.43 }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    ASSERT_EQUAL(ret, CJSON_PARSE_SUCCESS);
    ASSERT_EQUAL(handler->value_count, 4);
    ASSERT_EQUAL(handler->value_integer_count, 1);
    ASSERT_EQUAL(handler->value_bool_count, 1);
    ASSERT_EQUAL(handler->value_string_count, 1);
    ASSERT_EQUAL(handler->value_real_count, 1);

    REFCDEC(parser);
}

void nohandler(TEST_CASE_ARGUMENTS) {
    JsonEventsParser * parser = new(JsonEventsParser, NULL);
    String * json = new(String, "{ }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    ASSERT_EQUAL(ret, CJSON_PARSE_NO_HANDLER);
    REFCDEC(parser);
}

void null_input(TEST_CASE_ARGUMENTS) {
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser,(JsonEventsHandler *) handler);
    int ret = call(parser, parse, NULL);
    ASSERT_EQUAL(ret, CJSON_PARSE_NO_INPUT);
    REFCDEC(parser);
}


void invalid_json_tc1(TEST_CASE_ARGUMENTS) {
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "123{ }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    ASSERT_EQUAL(ret, CJSON_PARSE_INVALID_JSON);
    REFCDEC(parser);
}

void invalid_json_tc2(TEST_CASE_ARGUMENTS) {
    JsonCountingEventsHandler * handler = REFCTMP(new(JsonCountingEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);
    String * json = new(String, "{ } 32");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    ASSERT_EQUAL(ret, CJSON_PARSE_INVALID_JSON);
    REFCDEC(parser);
}

void build_object(TEST_CASE_ARGUMENTS) {
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    const char * expected_json = "{\"name\":\"John Doe\",\"age\":45,\"is_active\":false,\"amount\":123.4300}";

    String * json = new(String, "{ \"name\": \"John Doe\", \"age\": 45, \"is_active\": false, \"amount\": 123.43 }");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    ASSERT_EQUAL(ret, CJSON_PARSE_SUCCESS);

    String * object_string = call(handler->object, to_string);
    ASSERT_STRINGS_EQUAL(call(object_string, to_cstring), expected_json);

    REFCDEC(object_string);
    REFCDEC(parser);
}


void build_complex_object(TEST_CASE_ARGUMENTS) {
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    const char * expected_json = "{\"name\":\"John Doe\",\"active\":false,\"age\":45,\"children\":[{\"name\":\"Alice\",\"age\":5,\"parent\":\"John Doe\"},{\"name\":\"Bob\",\"age\":3,\"parent\":\"John Doe\"}]}";

    String * json = new(String, "{\"name\": \"John Doe\",\n\"active\": false,\n\"age\": 45,\n\"children\": \n[\n{\"name\": \"Alice\",\n\"age\": 5,\"parent\": \"John Doe\"\n},\n{\"name\": \"Bob\",\n\"age\": 3,\n\"parent\": \"John Doe\"\n}\n]\n}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    printf("ret=%d\n", ret);
    REFCDEC(json_stream);
    REFCDEC(json);
    ASSERT_EQUAL(ret, CJSON_PARSE_SUCCESS);

    String * object_string = call(handler->object, to_string);
    ASSERT_STRINGS_EQUAL(call(object_string, to_cstring), expected_json);

    REFCDEC(object_string);
    REFCDEC(parser);
}

void build_string_array(TEST_CASE_ARGUMENTS) {
    JsonObjectBuilderEventsHandler * handler = REFCTMP(new(JsonObjectBuilderEventsHandler));
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *) handler);

    String * json = new(String, "{\"strings\":[\"hello\",\"world\",\"foo\"]}");
    InputStream * json_stream = new(StringInputStream, json);
    int ret = call(parser, parse, json_stream);
    REFCDEC(json_stream);
    REFCDEC(json);
    ASSERT_EQUAL(ret, CJSON_PARSE_SUCCESS);

    String * name = new(String, "strings");
    Object * value = call(handler->object, get_value, name);
    ASSERT_NOT_NULL(value);
    ASSERT_TRUE(type_equal(value, "List"));
    List * list = (List *) value;
    ASSERT_EQUAL(list->length, 3);

    String * s0 = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s0, to_cstring), "hello");
    REFCDEC(s0);

    String * s1 = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "world");
    REFCDEC(s1);

    String * s2 = call(list, get, 2);
    ASSERT_STRINGS_EQUAL(call(s2, to_cstring), "foo");
    REFCDEC(s2);

    REFCDEC(name);
    REFCDEC(value);
    REFCDEC(parser);
}

TEST_CASES_BEGIN
    TEST_CASE(testcase);
    TEST_CASE(nohandler);
    TEST_CASE(null_input);
    TEST_CASE(invalid_json_tc1);
    TEST_CASE(invalid_json_tc2);
    TEST_CASE(build_object);
    TEST_CASE(build_complex_object);
    TEST_CASE(build_string_array);
TEST_CASES_END


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


