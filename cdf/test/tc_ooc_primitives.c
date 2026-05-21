#include "ooc_primitives.h"
#include "test_framework.h"

void boolean_new(TEST_CASE_ARGUMENTS) {
    Boolean * b = new(Boolean, false);
    ASSERT_TRUE(type_equal(b, "Boolean"));
    ASSERT_NOT_NULL(b);
    ASSERT_FALSE(b->value);
    REFCDEC(b);
    b = new(Boolean, true);
    ASSERT_NOT_NULL(b);
    ASSERT_TRUE(b->value);
    REFCDEC(b);
}

void boolean_to_string(TEST_CASE_ARGUMENTS) {
    Boolean * bfalse = new(Boolean, false);
    Boolean * btrue = new(Boolean, true);
    String * sfalse = call(bfalse, to_string);
    String * strue = call(btrue, to_string);

    ASSERT_STRINGS_EQUAL(call(sfalse, to_cstring), "false");
    ASSERT_STRINGS_EQUAL(call(strue, to_cstring), "true");

    REFCDEC(bfalse);
    REFCDEC(btrue);
    REFCDEC(sfalse);
    REFCDEC(strue);
}

void boolean_equals(TEST_CASE_ARGUMENTS) {
    Boolean * bfalse = new(Boolean, false);
    Boolean * btrue = new(Boolean, true);
    Boolean * b = new(Boolean, true);

    ASSERT_TRUE(call(b, equals, btrue));
    ASSERT_TRUE(call(btrue, equals, b));
    ASSERT_TRUE(call(btrue, equals, btrue));
    ASSERT_FALSE(call(b, equals, bfalse));
    ASSERT_FALSE(call(btrue, equals, bfalse));
    ASSERT_FALSE(call(btrue, equals, NULL));
    ASSERT_FALSE(call(bfalse, equals, NULL));

    Object * o = new(Object);
    String * s = new(String, "false");

    ASSERT_FALSE(call(o, equals, b));
    ASSERT_FALSE(call(b, equals, o));
    ASSERT_FALSE(call(s, equals, b));
    ASSERT_FALSE(call(b, equals, s));
    ASSERT_FALSE(call(bfalse, equals, s));

    REFCDEC(o);
    REFCDEC(s);
    REFCDEC(b);
    REFCDEC(bfalse);
    REFCDEC(btrue);
}

void boolean_from_string(TEST_CASE_ARGUMENTS) {
    Boolean * b = new(Boolean);
    String * s = new (String, "true");
    bool ok = call(b, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_TRUE(b->value);

    call(s, set_text, "false");
    ok = call(b, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_FALSE(b->value);

    call(s, set_text, "truee");
    ok = call(b, from_string, s);
    ASSERT_FALSE(ok);
    ASSERT_FALSE(b->value);

    REFCDEC(s);
    REFCDEC(b);
}

void integer_add(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 5);
    ASSERT_TRUE(type_equal(i, "Integer"));
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 5);

    Integer * res = call(i, add_int, 4);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 9);

    Integer * j = new(Integer, 7);
    REFCDEC(res);
    res = call(i, add, j);
    ASSERT_EQUAL(res->value, 12);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void integer_increment(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 5);
    Integer * j = new(Integer, 7);

    call(i, increment_int, 4);
    ASSERT_EQUAL(i->value, 9);
    call(i, increment, j);
    ASSERT_EQUAL(i->value, 16);

    REFCDEC(i);
    REFCDEC(j);
}

void integer_subtract(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 5);
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 5);

    Integer * res = call(i, subtract_int, 4);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 1);

    Integer * j = new(Integer, 7);
    REFCDEC(res);
    res = call(i, subtract, j);
    ASSERT_EQUAL(res->value, -2);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void integer_decrement(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 5);
    Integer * j = new(Integer, 7);

    call(i, increment_int, 4);
    ASSERT_EQUAL(i->value, 9);
    call(i, increment, j);
    ASSERT_EQUAL(i->value, 16);

    REFCDEC(i);
    REFCDEC(j);
}

void integer_multiply(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 5);
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 5);

    Integer * res = call(i, multiply_int, 4);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 20);

    Integer * j = new(Integer, 7);
    REFCDEC(res);
    res = call(i, multiply, j);
    ASSERT_EQUAL(res->value, 35);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void integer_multiply_inplace(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 5);
    Integer * j = new(Integer, 7);

    call(i, multiply_inplace_int, 4);
    ASSERT_EQUAL(i->value, 20);
    call(i, multiply_inplace, j);
    ASSERT_EQUAL(i->value, 140);

    REFCDEC(i);
    REFCDEC(j);
}

void integer_divide(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 25);
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 25);

    Integer * res = call(i, divide_int, 5);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 5);

    Integer * j = new(Integer, 10);
    REFCDEC(res);
    res = call(i, divide, j);
    ASSERT_EQUAL(res->value, 2);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void integer_divide_inplace(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 50);
    Integer * j = new(Integer, -10);

    call(i, divide_inplace_int, 5);
    ASSERT_EQUAL(i->value, 10);
    call(i, divide_inplace, j);
    ASSERT_EQUAL(i->value, -1);

    REFCDEC(i);
    REFCDEC(j);
}

void integer_equals(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, 1);
    Integer * j = new(Integer, -1);
    Integer * k = new(Integer, 1);

    ASSERT_TRUE(call(i, equals, i));
    ASSERT_TRUE(call(i, equals, k));
    ASSERT_TRUE(call(k, equals, i));
    ASSERT_FALSE(call(i, equals, j));
    ASSERT_FALSE(call(j, equals, i));
    ASSERT_FALSE(call(i, equals, NULL));

    Object * o = new(Object);
    String * s = new(String);
    ASSERT_FALSE(call(i, equals, o));
    ASSERT_FALSE(call(o, equals, i));
    ASSERT_FALSE(call(i, equals, s));
    ASSERT_FALSE(call(s, equals, i));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(k);
    REFCDEC(s);
    REFCDEC(o);
}

void integer_to_string(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer, -1234);
    String * s = call(i, to_string);
    ASSERT_STRINGS_EQUAL(call(s,to_cstring) ,"-1234");

    REFCDEC(i);
    REFCDEC(s);
}

void integer_from_string(TEST_CASE_ARGUMENTS) {
    Integer * i = new(Integer);
    String * s = new(String, "123");

    bool ok = call(i, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_EQUAL(i->value, 123);

    call(s, set_text, "-1234");
    ok = call(i, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_EQUAL(i->value, -1234);

    call(s, set_text, "abc");
    ok = call(i, from_string, s);
    ASSERT_FALSE(ok);
    ASSERT_EQUAL(i->value, -1234);


    REFCDEC(s);
    REFCDEC(i);
}

void long_add(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 5L);
    ASSERT_TRUE(type_equal(i, "Long"));
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 5L);

    Long * res = call(i, add_long, 4L);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 9L);

    Long * j = new(Long, 7L);
    REFCDEC(res);
    res = call(i, add, j);
    ASSERT_EQUAL(res->value, 12L);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void long_increment(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 5L);
    Long * j = new(Long, 7L);

    call(i, increment_long, 4L);
    ASSERT_EQUAL(i->value, 9L);
    call(i, increment, j);
    ASSERT_EQUAL(i->value, 16L);

    REFCDEC(i);
    REFCDEC(j);
}

void long_subtract(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 5L);
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 5L);

    Long * res = call(i, subtract_long, 4L);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 1L);

    Long * j = new(Long, 7L);
    REFCDEC(res);
    res = call(i, subtract, j);
    ASSERT_EQUAL(res->value, -2L);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void long_decrement(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 5L);
    Long * j = new(Long, 7L);

    call(i, increment_long, 4L);
    ASSERT_EQUAL(i->value, 9L);
    call(i, increment, j);
    ASSERT_EQUAL(i->value, 16L);

    REFCDEC(i);
    REFCDEC(j);
}

void long_multiply(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 5L);
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 5L);

    Long * res = call(i, multiply_long, 4L);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 20L);

    Long * j = new(Long, 7L);
    REFCDEC(res);
    res = call(i, multiply, j);
    ASSERT_EQUAL(res->value, 35L);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void long_multiply_inplace(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 5L);
    Long * j = new(Long, 7L);

    call(i, multiply_inplace_long, 4L);
    ASSERT_EQUAL(i->value, 20L);
    call(i, multiply_inplace, j);
    ASSERT_EQUAL(i->value, 140L);

    REFCDEC(i);
    REFCDEC(j);
}

void long_divide(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 25L);
    ASSERT_NOT_NULL(i);
    ASSERT_EQUAL(i->value, 25L);

    Long * res = call(i, divide_long, 5L);
    ASSERT_NOT_NULL(res);
    ASSERT_EQUAL(res->value, 5L);

    Long * j = new(Long, 10L);
    REFCDEC(res);
    res = call(i, divide, j);
    ASSERT_EQUAL(res->value, 2L);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void long_divide_inplace(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 50L);
    Long * j = new(Long, -10L);

    call(i, divide_inplace_long, 5L);
    ASSERT_EQUAL(i->value, 10L);
    call(i, divide_inplace, j);
    ASSERT_EQUAL(i->value, -1L);

    REFCDEC(i);
    REFCDEC(j);
}

void long_equals(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, 1L);
    Long * j = new(Long, -1L);
    Long * k = new(Long, 1L);

    ASSERT_TRUE(call(i, equals, i));
    ASSERT_TRUE(call(i, equals, k));
    ASSERT_TRUE(call(k, equals, i));
    ASSERT_FALSE(call(i, equals, j));
    ASSERT_FALSE(call(j, equals, i));
    ASSERT_FALSE(call(i, equals, NULL));

    Object * o = new(Object);
    String * s = new(String);
    ASSERT_FALSE(call(i, equals, o));
    ASSERT_FALSE(call(o, equals, i));
    ASSERT_FALSE(call(i, equals, s));
    ASSERT_FALSE(call(s, equals, i));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(k);
    REFCDEC(s);
    REFCDEC(o);
}

void long_to_string(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long, -1234L);
    String * s = call(i, to_string);
    ASSERT_STRINGS_EQUAL(call(s,to_cstring) ,"-1234");

    REFCDEC(i);
    REFCDEC(s);
}

void long_from_string(TEST_CASE_ARGUMENTS) {
    Long * i = new(Long);
    String * s = new(String, "123");

    bool ok = call(i, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_EQUAL(i->value, 123L);

    call(s, set_text, "-1234");
    ok = call(i, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_EQUAL(i->value, -1234L);

    call(s, set_text, "abc");
    ok = call(i, from_string, s);
    ASSERT_FALSE(ok);
    ASSERT_EQUAL(i->value, -1234L);


    REFCDEC(s);
    REFCDEC(i);
}

void double_add(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 5.5);
    ASSERT_TRUE(type_equal(i, "Double"));
    ASSERT_NOT_NULL(i);
    ASSERT_ALMOST_EQUAL(i->value, 5.5);

    Double * res = call(i, add_double, 4.0);
    ASSERT_NOT_NULL(res);
    ASSERT_ALMOST_EQUAL(res->value, 9.5);

    Double * j = new(Double, 7.34);
    REFCDEC(res);
    res = call(i, add, j);
    ASSERT_ALMOST_EQUAL(res->value, 12.84);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void double_increment(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 5.1);
    Double * j = new(Double, 7.2);

    call(i, increment_double, 4.2);
    ASSERT_ALMOST_EQUAL(i->value, 9.3);
    call(i, increment, j);
    ASSERT_ALMOST_EQUAL(i->value, 16.5);

    REFCDEC(i);
    REFCDEC(j);
}

void double_subtract(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 5.1);
    ASSERT_NOT_NULL(i);
    ASSERT_ALMOST_EQUAL(i->value, 5.1);

    Double * res = call(i, subtract_double, 4.2);
    ASSERT_NOT_NULL(res);
    ASSERT_ALMOST_EQUAL(res->value, 0.9);

    Double * j = new(Double, 7.3);
    REFCDEC(res);
    res = call(i, subtract, j);
    ASSERT_ALMOST_EQUAL(res->value, -2.2);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void double_decrement(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 5.6);
    Double * j = new(Double, 0.2);

    call(i, increment_double, 1.5);
    ASSERT_ALMOST_EQUAL(i->value, 7.1);
    call(i, increment, j);
    ASSERT_ALMOST_EQUAL(i->value, 7.3);

    REFCDEC(i);
    REFCDEC(j);
}

void double_multiply(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 5);
    ASSERT_NOT_NULL(i);
    ASSERT_ALMOST_EQUAL(i->value, 5.0);

    Double * res = call(i, multiply_double, 4.1);
    ASSERT_NOT_NULL(res);
    ASSERT_ALMOST_EQUAL(res->value, 20.5);

    Double * j = new(Double, 0.8);
    REFCDEC(res);
    res = call(i, multiply, j);
    ASSERT_ALMOST_EQUAL(res->value, 4);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void double_multiply_inplace(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 4);
    Double * j = new(Double, 7.5);

    call(i, multiply_inplace_double, 4L);
    ASSERT_ALMOST_EQUAL(i->value, 16);
    call(i, multiply_inplace, j);
    ASSERT_ALMOST_EQUAL(i->value, 120);

    REFCDEC(i);
    REFCDEC(j);
}

void double_divide(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 25.1);
    ASSERT_NOT_NULL(i);
    ASSERT_ALMOST_EQUAL(i->value, 25.1);

    Double * res = call(i, divide_double, 5);
    ASSERT_NOT_NULL(res);
    ASSERT_ALMOST_EQUAL(res->value, 5.02);

    Double * j = new(Double, 10);
    REFCDEC(res);
    res = call(i, divide, j);
    ASSERT_ALMOST_EQUAL(res->value, 2.51);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

void double_divide_inplace(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 50.5);
    Double * j = new(Double, -10.1);

    call(i, divide_inplace_double, 5);
    ASSERT_ALMOST_EQUAL(i->value, 10.1);
    call(i, divide_inplace, j);
    ASSERT_ALMOST_EQUAL(i->value, -1);

    REFCDEC(i);
    REFCDEC(j);
}

void double_equals(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, 1.23);
    Double * j = new(Double, -1.23);
    Double * k = new(Double, 1.23);

    ASSERT_TRUE(call(i, equals, i));
    ASSERT_TRUE(call(i, equals, k));
    ASSERT_TRUE(call(k, equals, i));
    ASSERT_FALSE(call(i, equals, j));
    ASSERT_FALSE(call(j, equals, i));
    ASSERT_FALSE(call(i, equals, NULL));

    Object * o = new(Object);
    String * s = new(String);
    ASSERT_FALSE(call(i, equals, o));
    ASSERT_FALSE(call(o, equals, i));
    ASSERT_FALSE(call(i, equals, s));
    ASSERT_FALSE(call(s, equals, i));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(k);
    REFCDEC(s);
    REFCDEC(o);
}

void double_to_string(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double, -12.34);
    String * s = call(i, to_string);
    ASSERT_STRINGS_EQUAL(call(s,to_cstring) ,"-12.3400");

    REFCDEC(i);
    REFCDEC(s);
}

void double_from_string(TEST_CASE_ARGUMENTS) {
    Double * i = new(Double);
    String * s = new(String, "1.23");

    bool ok = call(i, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_ALMOST_EQUAL(i->value, 1.23);

    call(s, set_text, "-12.34");
    ok = call(i, from_string, s);
    ASSERT_TRUE(ok);
    ASSERT_ALMOST_EQUAL(i->value, -12.34);

    call(s, set_text, "abc");
    ok = call(i, from_string, s);
    ASSERT_FALSE(ok);
    ASSERT_ALMOST_EQUAL(i->value, -12.34);


    REFCDEC(s);
    REFCDEC(i);
}

TEST_CASES_BEGIN
    TEST_CASE(boolean_new);
    TEST_CASE(boolean_equals);
    TEST_CASE(boolean_to_string);
    TEST_CASE(boolean_from_string);
    TEST_CASE(integer_add);
    TEST_CASE(integer_increment);
    TEST_CASE(integer_subtract);
    TEST_CASE(integer_decrement);
    TEST_CASE(integer_multiply);
    TEST_CASE(integer_multiply_inplace);
    TEST_CASE(integer_divide);
    TEST_CASE(integer_divide_inplace);
    TEST_CASE(integer_equals);
    TEST_CASE(integer_to_string);
    TEST_CASE(integer_from_string);
    TEST_CASE(long_add);
    TEST_CASE(long_increment);
    TEST_CASE(long_subtract);
    TEST_CASE(long_decrement);
    TEST_CASE(long_multiply);
    TEST_CASE(long_multiply_inplace);
    TEST_CASE(long_divide);
    TEST_CASE(long_divide_inplace);
    TEST_CASE(long_equals);
    TEST_CASE(long_to_string);
    TEST_CASE(long_from_string);
    TEST_CASE(double_add);
    TEST_CASE(double_increment);
    TEST_CASE(double_subtract);
    TEST_CASE(double_decrement);
    TEST_CASE(double_multiply);
    TEST_CASE(double_multiply_inplace);
    TEST_CASE(double_divide);
    TEST_CASE(double_divide_inplace);
    TEST_CASE(double_equals);
    TEST_CASE(double_to_string);
    TEST_CASE(double_from_string);
TEST_CASES_END

