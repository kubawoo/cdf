#include "test_framework.h"
#include "ooc_stringtokenizer.h"


void string_split_by_char(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "a,b,c");
    StringTokenizer * st = new(StringTokenizer, s);
    REFCDEC(s);
    List* l = call(st, split_by_char, ',');
    REFCDEC(st);
    ASSERT_EQUAL(l->length, 3);

    String * e = call(l, get, 0);
    ASSERT_STRINGS_EQUAL(call(e, to_cstring), "a");
    REFCDEC(e);

    e = call(l, get, 1);
    ASSERT_STRINGS_EQUAL(call(e, to_cstring), "b");
    REFCDEC(e);

    e = call(l, get, 2);
    ASSERT_STRINGS_EQUAL(call(e, to_cstring), "c");
    REFCDEC(e);

    REFCDEC(l);
}

void string_split_by_char2(TEST_CASE_ARGUMENTS) {
    String * s = new(String, ",,,,a,b,,c,");
    StringTokenizer * st = new(StringTokenizer, s);
    REFCDEC(s);
    List* l = call(st, split_by_char, ',');
    REFCDEC(st);
    ASSERT_EQUAL(l->length, 3);

    String * e = call(l, get, 0);
    ASSERT_STRINGS_EQUAL(call(e, to_cstring), "a");
    REFCDEC(e);

    e = call(l, get, 1);
    ASSERT_STRINGS_EQUAL(call(e, to_cstring), "b");
    REFCDEC(e);

    e = call(l, get, 2);
    ASSERT_STRINGS_EQUAL(call(e, to_cstring), "c");
    REFCDEC(e);

    REFCDEC(l);
}

TEST_CASES_BEGIN
    TEST_CASE(string_split_by_char);
    TEST_CASE(string_split_by_char2);
TEST_CASES_END

