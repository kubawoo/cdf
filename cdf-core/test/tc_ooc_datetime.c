#include "test_framework.h"
#include "ooc_datetime.h"

void constructor_test(TEST_CASE_ARGUMENTS) {
    DateTime * dt = new(DateTime);
    ASSERT_NOT_NULL(dt);
    ASSERT_TRUE(type_equal(dt, "DateTime"));
    String * s = call(dt, to_string);
    ASSERT_NOT_NULL(s);

    delete(dt);
    delete(s);
}

void to_string_test1(TEST_CASE_ARGUMENTS) {
    DateTime * dt = new(DateTime, 2001, 1, 1);
    String * s = call(dt, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "2001-01-01T00:00:00");
    delete(s);
    delete(dt);
}

void to_string_test2(TEST_CASE_ARGUMENTS) {
    DateTime * dt = new(DateTime, 2001, 12, 21, 14, 55, 3);
    String * s = call(dt, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "2001-12-21T14:55:03");
    delete(s);
    delete(dt);
}

void http_format(TEST_CASE_ARGUMENTS) {
    DateTime * dt = new(DateTime, 1994, 11, 15, 8, 12, 31);
    String * s = call(dt, format, "%a, %d %b %Y %H:%M:%S");
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "Tue, 15 Nov 1994 08:12:31");
    delete(s);
    delete(dt); 
}

TEST_CASES_BEGIN
    TEST_CASE(constructor_test);
    TEST_CASE(to_string_test1);
    TEST_CASE(to_string_test2);
    TEST_CASE(http_format);
TEST_CASES_END

