#include "test_framework.h"
#include "ooc.h"
#include "ooc_list.h"

void list_test(TEST_CASE_ARGUMENTS) {
    List * list = new(List);
    ASSERT_TRUE(type_equal(list, "List"));
    ASSERT_EQUAL(list->length, 0);
    ASSERT_NULL(list->_first);
    ASSERT_NULL(list->_last);

    String * s = new(String, "hello");
    call(list, add, s);
    REFCDEC(s);

    ASSERT_EQUAL(list->length, 1);
    ASSERT_NOT_NULL(list->_first);
    ASSERT_POINTERS_EQUAL(list->_first, list->_last);
    ASSERT_POINTERS_EQUAL(list->_first->item, s);

    String * s2 = new(String, "world");
    call(list, add, s2);
    REFCDEC(s2);
    ASSERT_EQUAL(list->length, 2);
    ASSERT_NOT_NULL(list->_first);
    ASSERT_NOT_NULL(list->_last);
    ASSERT_POINTERS_NOT_EQUAL(list->_first, list->_last);
    ASSERT_POINTERS_EQUAL(list->_first->item, s);
    ASSERT_POINTERS_EQUAL(list->_last->item, s2);
    REFCDEC(list);
}


void list_get(TEST_CASE_ARGUMENTS) {
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "0")));
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "2")));
    call(list, add, REFCTMP(new(String, "3")));
    call(list, add, REFCTMP(new(String, "4")));

    ASSERT_EQUAL(list->length, 5);
    for(int i = 0; i < 5; ++i) {
        char buffer[2] = "x";
        buffer[0] = '0' + i;
        String * s = call(list, get, i);
        ASSERT_STRINGS_EQUAL(call(s, to_cstring), buffer);
        REFCDEC(s);
    }

    ASSERT_NULL(call(list, get, -1));
    ASSERT_NULL(call(list, get, 5));
    ASSERT_NULL(call(list, get, 1234));

    ASSERT_EQUAL(list->length, 5);
    REFCDEC(list);
}

void list_set(TEST_CASE_ARGUMENTS) {
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "0")));
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "2")));
    call(list, add, REFCTMP(new(String, "3")));
    call(list, add, REFCTMP(new(String, "4")));

    ASSERT_EQUAL(list->length, 5);

    call(list, set, 0, REFCTMP(new(String, "a")));
    call(list, set, 2, REFCTMP(new(String, "c")));
    call(list, set, 4, REFCTMP(new(String, "e")));
    for(int i = 0; i < 5; ++i) {
        char buffer[2] = "x";
        if(i % 2) {
            buffer[0] = '0' + i;
        } else {
            buffer[0] = 'a' + i;
        }
        String * s = call(list, get, i);
        ASSERT_STRINGS_EQUAL(call(s, to_cstring), buffer);
        REFCDEC(s);
    }

    ASSERT_EQUAL(list->length, 5);
    REFCDEC(list);
}


void list_remove(TEST_CASE_ARGUMENTS) {
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "0")));
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "2")));
    call(list, add, REFCTMP(new(String, "3")));
    call(list, add, REFCTMP(new(String, "4")));

    ASSERT_EQUAL(list->length, 5);

    call(list, remove, 0);

    String * s = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");
    REFCDEC(s);
    s = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "2");
    REFCDEC(s);
    s = call(list, get, 2);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "3");
    REFCDEC(s);
    s = call(list, get, 3);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "4");
    REFCDEC(s);
    ASSERT_NULL(call(list, get, 4));

    call(list, remove, 1);

    s = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");
    REFCDEC(s);
    s = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "3");
    REFCDEC(s);
    s = call(list, get, 2);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "4");
    REFCDEC(s);
    ASSERT_NULL(call(list, get, 3));


    call(list, remove, 2);

    s = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");
    REFCDEC(s);
    s = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "3");
    REFCDEC(s);
    ASSERT_NULL(call(list, get, 2));

    REFCDEC(list);
}

void list_insert(TEST_CASE_ARGUMENTS) {
    List * list = new(List);
    call(list, add, REFCTMP(new(String, "1")));
    call(list, add, REFCTMP(new(String, "3")));

    ASSERT_EQUAL(list->length, 2);
    String * s = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");
    REFCDEC(s);
    s = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "3");
    REFCDEC(s);
    ASSERT_NULL(call(list, get, 3));

    call(list, insert, 2, (Object*) REFCTMP(new(String, "4")));

    ASSERT_EQUAL(list->length, 3);
    s = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");
    REFCDEC(s);
    s = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "3");
    REFCDEC(s);
    s = call(list, get, 2);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "4");
    REFCDEC(s);
    ASSERT_NULL(call(list, get, 3));

    call(list, insert, 0, (Object*) REFCTMP(new(String, "0")));

    ASSERT_EQUAL(list->length, 4);
    s = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "0");
    REFCDEC(s);
    s = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");
    REFCDEC(s);
    s = call(list, get, 2);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "3");
    REFCDEC(s);
    s = call(list, get, 3);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "4");
    REFCDEC(s);
    ASSERT_NULL(call(list, get, 4));

    call(list, insert, 2, (Object*) REFCTMP(new(String, "2")));

    ASSERT_EQUAL(list->length, 5);
    s = call(list, get, 0);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "0");
    REFCDEC(s);
    s = call(list, get, 1);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "1");
    REFCDEC(s);
    s = call(list, get, 2);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "2");
    REFCDEC(s);
    s = call(list, get, 3);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "3");
    REFCDEC(s);
    s = call(list, get, 4);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "4");
    REFCDEC(s);
    ASSERT_NULL(call(list, get, 5));
    REFCDEC(list);
}

void List_contains(TEST_CASE_ARGUMENTS) {
	List * list = new(List);
	String * a = new(String, "a");
	String * a2 = new(String, "a");
	String * b = new(String, "b");


	ASSERT_FALSE(call(list, contains, a));
	ASSERT_FALSE(call(list, contains, a2));
	ASSERT_FALSE(call(list, contains, b));

	call(list, add, a);
	ASSERT_TRUE(call(list, contains, a));
	ASSERT_TRUE(call(list, contains, a2));
	ASSERT_FALSE(call(list, contains, b));

	call(list, add, b);
	ASSERT_TRUE(call(list, contains, a));
	ASSERT_TRUE(call(list, contains, a2));
	ASSERT_TRUE(call(list, contains, b));

	call(list, remove, 0);
	ASSERT_FALSE(call(list, contains, a));
	ASSERT_FALSE(call(list, contains, a2));
	ASSERT_TRUE(call(list, contains, b));

	call(list, remove, 0);
	ASSERT_FALSE(call(list, contains, a));
	ASSERT_FALSE(call(list, contains, a2));
	ASSERT_FALSE(call(list, contains, b));

	REFCDEC(list);
	REFCDEC(a);
	REFCDEC(a2);
	REFCDEC(b);
}

void list_to_string(TEST_CASE_ARGUMENTS) {
	List * l = new(List);
	String * s = call(l, to_string);
	ASSERT_STRINGS_EQUAL(call(s, to_cstring), "[]");
	REFCDEC(s);

	call(l, add, REFCTMP(new(String, "hi")));
	s = call(l, to_string);
	ASSERT_STRINGS_EQUAL(call(s, to_cstring), "[hi]");
	REFCDEC(s);

	call(l, add, REFCTMP(new(String, "hello")));
	s = call(l, to_string);
	ASSERT_STRINGS_EQUAL(call(s, to_cstring), "[hi; hello]");
	REFCDEC(s);

	call(l, add, NULL);
	s = call(l, to_string);
	ASSERT_STRINGS_EQUAL(call(s, to_cstring), "[hi; hello; NULL]");
	REFCDEC(s);

	REFCDEC(l);
}

TEST_CASES_BEGIN
    TEST_CASE(list_test);
    TEST_CASE(list_get);
    TEST_CASE(list_set);
    TEST_CASE(list_remove);
    TEST_CASE(list_insert);
    TEST_CASE(list_to_string);
TEST_CASES_END

