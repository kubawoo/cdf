#include "test_framework.h"
#include "ooc_object.h"

void object_equals(TEST_CASE_ARGUMENTS) {
    Object * o1 = new(Object);
    ASSERT_TRUE(type_equal(o1, "Object"));
    Object * o2 = new(Object);

    ASSERT_TRUE(call(o1, equals, o1));
    ASSERT_TRUE(call(o2, equals, o2));
    ASSERT_FALSE(call(o2, equals, o1));
    ASSERT_FALSE(call(o1, equals, o2));
    ASSERT_FALSE(call(o1, equals, NULL));

    delete(o1);
    delete(o2);
}

void object_copy(TEST_CASE_ARGUMENTS) {
    Object * o = new(Object);
    Object * copy = call(o, copy);
    ASSERT_NULL(copy);

    delete(o);
}

void object_refc(TEST_CASE_ARGUMENTS) {
    Object * o = new(Object);
    ASSERT_EQUAL(o->_refc, 1);
    REFCINC(o);
    ASSERT_EQUAL(o->_refc, 2);
    REFCDEC(o);
    ASSERT_EQUAL(o->_refc, 1);
    REFCDEC(o);
    ASSERT_NULL(o);

    o = REFCTMP(new(Object));
    ASSERT_NOT_NULL(o);
    ASSERT_EQUAL(o->_refc, 0);
    REFCINC(o);
    ASSERT_EQUAL(o->_refc, 1);
    REFCDEC(o);
    ASSERT_NULL(o);

    o = REFCTMP(new(Object));
    ASSERT_NOT_NULL(o);
    ASSERT_EQUAL(o->_refc, 0);
    REFCDEC(o);
    ASSERT_NULL(o);
}

TEST_CASES_BEGIN
    TEST_CASE(object_equals);
    TEST_CASE(object_copy);
    TEST_CASE(object_refc);
TEST_CASES_END
