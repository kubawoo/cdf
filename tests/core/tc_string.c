#include "thestring.h"
#include <assert.h>


static void string_new(void)
{
    String * s = new(String);
    assert(type_equal(s, "String"));
    assert((s->length) == (0));

    call(s, set_text, "12345");
    assert((s->length) == (5));
    assert(s->_content != NULL);
    assert((s->_allocated) == (8));
    assert(strcmp(call(s, to_cstring), "12345") == 0);

    call(s, set_text, "");
    assert((s->length) == (0));
    assert(s->_content != NULL);
    assert((s->_allocated) == (8));
    assert(strcmp(call(s, to_cstring), "") == 0);
    REFCDEC(s);
}

static void string_append(void)
{
    String *s1 = new(String, "Hello");
    call(s1, append_cstring, " ");
    String *s2 = new(String, "world!");
    call(s1, append, s2);

    assert((s1->length) == (12));
    assert(s1->_content != NULL);
    assert((s1->_allocated) == (16));
    assert(strcmp(call(s1, to_cstring), "Hello world!") == 0);
    REFCDEC(s2);
    assert(strcmp(call(s1, to_cstring), "Hello world!") == 0);
    REFCDEC(s1);
}

static void string_append_char(void)
{
    String *s1 = new(String, "Hello");
    call(s1, append_char, ' ');
    String *s2 = new(String, "world!");
    call(s1, append, s2);

    assert((s1->length) == (12));
    assert(s1->_content != NULL);
    assert((s1->_allocated) == (16));
    assert(strcmp(call(s1, to_cstring), "Hello world!") == 0);
    REFCDEC(s2);
    REFCDEC(s1);
}

static void string_append_char_to_empty_string(void)
{
    String *s = new(String);
    call(s, append_char, 'a');
    assert((s->length) == (1));
    assert(s->_content != NULL);
    assert((s->_allocated) == (2));
    assert(strcmp(call(s, to_cstring), "a") == 0);
    REFCDEC(s);
}

static void string_equals(void)
{
    String * s1 = new(String, "abc");
    String * s2 = new(String, "def");
    String * s3 = new(String, "abc");

    assert(call(s1, equals, s1));
    assert(!(call(s1, equals, s2)));
    assert(call(s1, equals, s3));
    assert(!(call(s2, equals, s1)));
    assert(call(s3, equals, s1));

    assert(!(call(s1, equals, NULL)));

    Object * o = new(Object);
    assert(!(call(s1, equals, o)));
    assert(!(call(o, equals, s1)));

    REFCDEC(s1);
    REFCDEC(s2);
    REFCDEC(s3);
    REFCDEC(o);
}

static void string_equals_cstring(void)
{
    String * s = new(String, "abcd");
    assert(call(s, equals_cstring, "abcd"));
    assert(!(call(s, equals_cstring, " abcd")));
    assert(!(call(s, equals_cstring, "abcd ")));
    assert(!(call(s, equals_cstring, "abcde")));
    assert(!(call(s, equals_cstring, "aabcd")));
    assert(!(call(s, equals_cstring, NULL)));
    assert(!(call(s, equals_cstring, "")));
    REFCDEC(s);
}

static void string_contains_any(void)
{
    String * s1 = new(String, "abcdef");
    String * s2 = new(String, "qwerty");
    String * s3 = new(String, "uiop");

    assert(!(call(s3, contains_any, s1)));
    assert(!(call(s1, contains_any, s3)));
    assert(call(s2, contains_any, s1));
    assert(call(s1, contains_any, s2));
    assert(call(s1, contains_any_char, "qaz"));

    REFCDEC(s1);
    REFCDEC(s2);
    REFCDEC(s3);
}

static void string_copy(void)
{
    String * s = new(String, "abcd");
    String * copy = (String *) call(s, copy);
    assert(copy != NULL);
    assert((void*)(s) != (void*)(copy));
    assert(strcmp(call(s, to_cstring), call(copy, to_cstring)) == 0);

    REFCDEC(s);
    REFCDEC(copy);
}

static void string_index_of(void)
{
    String * s = new(String, "abcdefabcdef");
    assert((call(s, index_of_char, 'a')) == (0));
    assert((call(s, index_of_char, 'e')) == (4));
    assert((call(s, index_of_char, 'h')) == (-1));
    assert((call(s, index_of_char, '\0')) == (-1));
    assert((call(s, index_of_cstring, "a")) == (0));
    assert((call(s, index_of_cstring, "ab")) == (0));
    assert((call(s, index_of_cstring, "abc")) == (0));
    assert((call(s, index_of_cstring, "bc")) == (1));
    assert((call(s, index_of_cstring, "fa")) == (5));
    assert((call(s, index_of_cstring, "ac")) == (-1));
    assert((call(s, index_of_cstring, "bcdef")) == (1));
    assert((call(s, index_of_cstring, "bac")) == (-1));
    assert((call(s, index_of_cstring, "")) == (-1));
    assert((call(s, index_of_cstring, "abcdefabcdef")) == (0));

    String * s2 = new(String, "def");
    assert((call(s, index_of_string, s2)) == (3));

    assert((call(s, next_index_of_char, 1, 'a')) == (6));
    assert((call(s, next_index_of_char, 6, 'a')) == (6));
    assert((call(s, next_index_of_char, 7, 'a')) == (-1));

    assert((call(s, next_index_of_cstring, 1, "abc")) == (6));
    assert((call(s, next_index_of_cstring, 6, "abc")) == (6));
    assert((call(s, next_index_of_cstring, 7, "abc")) == (-1));

    REFCDEC(s);
    REFCDEC(s2);
}

static void string_substring(void)
{
    String * s = new(String, "abcdefghijkl");
    String * s1 = call(s, substring, 0, 1);
    assert(strcmp(call(s1, to_cstring), "a") == 0);
    REFCDEC(s1);

    s1 = call(s, substring, 0, 2);
    assert(strcmp(call(s1, to_cstring), "ab") == 0);
    REFCDEC(s1);

    s1 = call(s, substring, 1, 4);
    assert(strcmp(call(s1, to_cstring), "bcd") == 0);
    REFCDEC(s1);

    s1 = call(s, substring_from, 4);
    assert(strcmp(call(s1, to_cstring), "efghijkl") == 0);
    REFCDEC(s1);

    REFCDEC(s);
}

static void string_format(void)
{
    String * s = new(String, "abcdefghijkl");
    assert((s->length) == (12));
    assert((s->_allocated) == (16));

    call(s, format, "%s", "1");
    assert((s->length) == (1));
    assert((s->_allocated) == (16));
    assert(strcmp(call(s, to_cstring), "1") == 0);

    call(s, format, "%s%d%s", "12", 34, "56");
    assert((s->length) == (6));
    assert((s->_allocated) == (16));
    assert(strcmp(call(s, to_cstring), "123456") == 0);

    call(s, format, "%s%d%s%s", "aa", 34, "bb", "78901234567890");
    assert((s->length) == (20));
    assert((s->_allocated) == (32));
    assert(strcmp(call(s, to_cstring), "aa34bb78901234567890") == 0);

    REFCDEC(s);
}

static void string_trim(void)
{
    String * s = new(String, " \n\tabcd\n\t  ");
    assert((s->length) == (11));
    call(s, trim);
    assert(strcmp("abcd", call(s, to_cstring)) == 0);
    assert((s->length) == (4));
    REFCDEC(s);
}


static void string_trim_left(void)
{
    String * s = new(String, " \n\tabcd");
    assert((s->length) == (7));
    call(s, trim);
    assert(strcmp("abcd", call(s, to_cstring)) == 0);
    assert((s->length) == (4));
    REFCDEC(s);
}

static void string_trim_right(void)
{
    String * s = new(String, "abcd \n\t");
    assert((s->length) == (7));
    call(s, trim);
    assert(strcmp("abcd", call(s, to_cstring)) == 0);
    assert((s->length) == (4));
    REFCDEC(s);
}

static void string_trim_empty(void)
{
    String * s = new(String, " \n\t");
    assert((s->length) == (3));
    assert(strcmp(" \n\t", call(s, to_cstring)) == 0);
    call(s, trim);
    assert(strcmp("", call(s, to_cstring)) == 0);
    assert((s->length) == (0));
    REFCDEC(s);
}
int main(void)
{
    string_new();
    string_append();
    string_append_char();
    string_append_char_to_empty_string();
    string_equals();
    string_equals_cstring();
    string_contains_any();
    string_copy();
    string_index_of();
    string_substring();
    string_format();
    string_trim();
    string_trim_left();
    string_trim_right();
    string_trim_empty();
    return 0;
}



