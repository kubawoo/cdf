#include "ooc_list.h"
#include "ooc_string.h"
#include "test_framework.h"

void string_new(TEST_CASE_ARGUMENTS) {
    String * s = new(String);
    ASSERT_TRUE(type_equal(s, "String"));
    ASSERT_EQUAL(s->length, 0);

    call(s, set_text, "12345");
    ASSERT_EQUAL(s->length, 5);
    ASSERT_NOT_NULL(s->_content);
    ASSERT_EQUAL(s->_allocated, 8);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "12345");

    call(s, set_text, "");
    ASSERT_EQUAL(s->length, 0);
    ASSERT_NOT_NULL(s->_content);
    ASSERT_EQUAL(s->_allocated, 8);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "");
    REFCDEC(s);
}

void string_append(TEST_CASE_ARGUMENTS) {
    String *s1 = new(String, "Hello");
    call(s1, append_cstring, " ");
    String *s2 = new(String, "world!");
    call(s1, append, s2);

    ASSERT_EQUAL(s1->length, 12);
    ASSERT_NOT_NULL(s1->_content);
    ASSERT_EQUAL(s1->_allocated, 16);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "Hello world!");
    REFCDEC(s2);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "Hello world!");
    REFCDEC(s1);
}

void string_append_char(TEST_CASE_ARGUMENTS) {
    String *s1 = new(String, "Hello");
    call(s1, append_char, ' ');
    String *s2 = new(String, "world!");
    call(s1, append, s2);

    ASSERT_EQUAL(s1->length, 12);
    ASSERT_NOT_NULL(s1->_content);
    ASSERT_EQUAL(s1->_allocated, 16);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "Hello world!");
    REFCDEC(s2);
    REFCDEC(s1);
}

void string_append_char_to_empty_string(TEST_CASE_ARGUMENTS) {
    String *s = new(String);
    call(s, append_char, 'a');
    ASSERT_EQUAL(s->length, 1);
    ASSERT_NOT_NULL(s->_content);
    ASSERT_EQUAL(s->_allocated, 2);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "a");
    REFCDEC(s);
}

void string_equals(TEST_CASE_ARGUMENTS) {
    String * s1 = new(String, "abc");
    String * s2 = new(String, "def");
    String * s3 = new(String, "abc");

    ASSERT_TRUE(call(s1, equals, s1));
    ASSERT_FALSE(call(s1, equals, s2));
    ASSERT_TRUE(call(s1, equals, s3));
    ASSERT_FALSE(call(s2, equals, s1));
    ASSERT_TRUE(call(s3, equals, s1));

    ASSERT_FALSE(call(s1, equals, NULL));

    Object * o = new(Object);
    ASSERT_FALSE(call(s1, equals, o));
    ASSERT_FALSE(call(o, equals, s1));

    REFCDEC(s1);
    REFCDEC(s2);
    REFCDEC(s3);
    REFCDEC(o);
}

void string_equals_cstring(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "abcd");
    ASSERT_TRUE(call(s, equals_cstring, "abcd"));
    ASSERT_FALSE(call(s, equals_cstring, " abcd"));
    ASSERT_FALSE(call(s, equals_cstring, "abcd "));
    ASSERT_FALSE(call(s, equals_cstring, "abcde"));
    ASSERT_FALSE(call(s, equals_cstring, "aabcd"));
    ASSERT_FALSE(call(s, equals_cstring, NULL));
    ASSERT_FALSE(call(s, equals_cstring, ""));
    REFCDEC(s);
}

void string_contains_any(TEST_CASE_ARGUMENTS) {
    String * s1 = new(String, "abcdef");
    String * s2 = new(String, "qwerty");
    String * s3 = new(String, "uiop");

    ASSERT_FALSE(call(s3, contains_any, s1));
    ASSERT_FALSE(call(s1, contains_any, s3));
    ASSERT_TRUE(call(s2, contains_any, s1));
    ASSERT_TRUE(call(s1, contains_any, s2));
    ASSERT_TRUE(call(s1, contains_any_char, "qaz"));

    REFCDEC(s1);
    REFCDEC(s2);
    REFCDEC(s3);
}

void string_copy(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "abcd");
    String * copy = (String *) call(s, copy);
    ASSERT_NOT_NULL(copy);
    ASSERT_POINTERS_NOT_EQUAL(s, copy);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), call(copy, to_cstring));

    REFCDEC(s);
    REFCDEC(copy);
}

void string_index_of(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "abcdefabcdef");
    ASSERT_EQUAL(call(s, index_of_char, 'a'), 0);
    ASSERT_EQUAL(call(s, index_of_char, 'e'), 4);
    ASSERT_EQUAL(call(s, index_of_char, 'h'), -1);
    ASSERT_EQUAL(call(s, index_of_char, '\0'), -1);
    ASSERT_EQUAL(call(s, index_of_cstring, "a"), 0);
    ASSERT_EQUAL(call(s, index_of_cstring, "ab"), 0);
    ASSERT_EQUAL(call(s, index_of_cstring, "abc"), 0);
    ASSERT_EQUAL(call(s, index_of_cstring, "bc"), 1);
    ASSERT_EQUAL(call(s, index_of_cstring, "fa"), 5);
    ASSERT_EQUAL(call(s, index_of_cstring, "ac"), -1);
    ASSERT_EQUAL(call(s, index_of_cstring, "bcdef"), 1);
    ASSERT_EQUAL(call(s, index_of_cstring, "bac"), -1);
    ASSERT_EQUAL(call(s, index_of_cstring, ""), -1);
    ASSERT_EQUAL(call(s, index_of_cstring, "abcdefabcdef"), 0);

    String * s2 = new(String, "def");
    ASSERT_EQUAL(call(s, index_of_string, s2), 3);

    ASSERT_EQUAL(call(s, next_index_of_char, 1, 'a'), 6);
    ASSERT_EQUAL(call(s, next_index_of_char, 6, 'a'), 6);
    ASSERT_EQUAL(call(s, next_index_of_char, 7, 'a'), -1);

    ASSERT_EQUAL(call(s, next_index_of_cstring, 1, "abc"), 6);
    ASSERT_EQUAL(call(s, next_index_of_cstring, 6, "abc"), 6);
    ASSERT_EQUAL(call(s, next_index_of_cstring, 7, "abc"), -1);

    REFCDEC(s);
    REFCDEC(s2);
}

void string_substring(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "abcdefghijkl");
    String * s1 = call(s, substring, 0, 1);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "a");
    REFCDEC(s1);

    s1 = call(s, substring, 0, 2);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "ab");
    REFCDEC(s1);

    s1 = call(s, substring, 1, 4);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "bcd");
    REFCDEC(s1);

    s1 = call(s, substring_from, 4);
    ASSERT_STRINGS_EQUAL(call(s1, to_cstring), "efghijkl");
    REFCDEC(s1);

    REFCDEC(s);
}

void string_format(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "abcdefghijkl");
    ASSERT_EQUAL(s->length, 12);
    ASSERT_EQUAL(s->_allocated, 16);

    call(s, format, "%s", "1");
    ASSERT_EQUAL(s->length, 1);
    ASSERT_EQUAL(s->_allocated, 16);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");

    call(s, format, "%s%d%s", "12", 34, "56");
    ASSERT_EQUAL(s->length, 6);
    ASSERT_EQUAL(s->_allocated, 16);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "123456");

    call(s, format, "%s%d%s%s", "aa", 34, "bb", "78901234567890");
    ASSERT_EQUAL(s->length, 20);
    ASSERT_EQUAL(s->_allocated, 32);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "aa34bb78901234567890");

    REFCDEC(s);
}

void string_trim(TEST_CASE_ARGUMENTS) {
    String * s = new(String, " \n\tabcd\n\t  ");
    ASSERT_EQUAL(s->length, 11);
    call(s, trim);
    ASSERT_STRINGS_EQUAL("abcd", call(s, to_cstring));
    ASSERT_EQUAL(s->length, 4);
    REFCDEC(s);
}


void string_trim_left(TEST_CASE_ARGUMENTS) {
    String * s = new(String, " \n\tabcd");
    ASSERT_EQUAL(s->length, 7);
    call(s, trim);
    ASSERT_STRINGS_EQUAL("abcd", call(s, to_cstring));
    ASSERT_EQUAL(s->length, 4);
    REFCDEC(s);
}

void string_trim_right(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "abcd \n\t");
    ASSERT_EQUAL(s->length, 7);
    call(s, trim);
    ASSERT_STRINGS_EQUAL("abcd", call(s, to_cstring));
    ASSERT_EQUAL(s->length, 4);
    REFCDEC(s);
}

void string_trim_empty(TEST_CASE_ARGUMENTS) {
    String * s = new(String, " \n\t");
    ASSERT_EQUAL(s->length, 3);
    ASSERT_STRINGS_EQUAL(" \n\t", call(s, to_cstring));
    call(s, trim);
    ASSERT_STRINGS_EQUAL("", call(s, to_cstring));
    ASSERT_EQUAL(s->length, 0);
    REFCDEC(s);
}


TEST_CASES_BEGIN
    TEST_CASE(string_new);
    TEST_CASE(string_append);
    TEST_CASE(string_append_char);
    TEST_CASE(string_append_char_to_empty_string);
    TEST_CASE(string_equals);
    TEST_CASE(string_equals_cstring);
    TEST_CASE(string_contains_any);
    TEST_CASE(string_copy);
    TEST_CASE(string_index_of);
    TEST_CASE(string_substring);
    TEST_CASE(string_format);
    TEST_CASE(string_trim);
    TEST_CASE(string_trim_left);
    TEST_CASE(string_trim_right);
    TEST_CASE(string_trim_empty);
TEST_CASES_END


