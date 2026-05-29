#include "ooc.h"
#include "ooc_array.h"
#include "test_framework.h"

void array_test(TEST_CASE_ARGUMENTS) {
    Array * a = new(Array, 10);
    ASSERT_TRUE(type_equal(a, "Array"));
    ASSERT_EQUAL(a->length, 10);
    ASSERT_NOT_NULL(a->_values);
    for(int i = 0; i < 10; ++i) {
        ASSERT_NULL(call(a, get, i));
    }

    call(a, set, 3, REFCTMP(new(String, "one two free")));

    ASSERT_EQUAL(a->length, 10);
    ASSERT_NOT_NULL(a->_values);
    for(int i = 0; i < 10; ++i) {
        Object * o = call(a, get, i);
        if(i == 3) {
            ASSERT_NOT_NULL(o);
            ASSERT_STRINGS_EQUAL("String", o->type);
            ASSERT_STRINGS_EQUAL(call((String *) o, to_cstring), "one two free");
        } else {
            ASSERT_NULL(o);
        }
        REFCDEC(o);
    }
    REFCDEC(a);
}

void array_iterator_test(TEST_CASE_ARGUMENTS) {
    Array * a = new(Array, 5);
    call(a, set, 0, REFCTMP(new(String, "zero")));
    call(a, set, 1, REFCTMP(new(String, "one")));
    call(a, set, 2, REFCTMP(new(String, "two")));
    call(a, set, 3, REFCTMP(new(String, "three")));
    call(a, set, 4, REFCTMP(new(String, "four")));

    Iterator * it = call(a, iterator);
    ASSERT_TRUE(type_equal(it, "ArrayIterator"));

    const char *expected[] = {"zero", "one", "two", "three", "four"};
    for(int i = 0; i < 5; ++i) {
        ASSERT_TRUE(call(it, hasNext));
        String * s = call(it, next);
        ASSERT_NOT_NULL(s);
        ASSERT_STRINGS_EQUAL(call(s, to_cstring), expected[i]);
        REFCDEC(s);
    }

    ASSERT_FALSE(call(it, hasNext));
    ASSERT_NULL(call(it, next));
    ASSERT_NULL(call(it, next));

    REFCDEC(it);
    REFCDEC(a);
}

void array_iterator_empty(TEST_CASE_ARGUMENTS) {
    Array * a = new(Array, 3);
    Iterator * it = call(a, iterator);
    ASSERT_TRUE(type_equal(it, "ArrayIterator"));
    ASSERT_TRUE(call(it, hasNext));
    ASSERT_NULL(call(it, next));
    ASSERT_TRUE(call(it, hasNext));
    ASSERT_NULL(call(it, next));
    ASSERT_TRUE(call(it, hasNext));
    ASSERT_NULL(call(it, next));
    ASSERT_FALSE(call(it, hasNext));
    ASSERT_NULL(call(it, next));
    REFCDEC(it);
    REFCDEC(a);
}

TEST_CASES_BEGIN
    TEST_CASE(array_test);
    TEST_CASE(array_iterator_test);
    TEST_CASE(array_iterator_empty);
TEST_CASES_END
