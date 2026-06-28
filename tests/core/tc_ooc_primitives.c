#include "ooc_primitives.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void boolean_new(void)
{
    Boolean * b = new(Boolean, false);
    assert(type_equal(b, "Boolean"));
    assert(b != NULL);
    assert(!(b->value));
    REFCDEC(b);
    b = new(Boolean, true);
    assert(b != NULL);
    assert(b->value);
    REFCDEC(b);
}

static void boolean_to_string(void)
{
    Boolean * bfalse = new(Boolean, false);
    Boolean * btrue = new(Boolean, true);
    String * sfalse = call(bfalse, to_string);
    String * strue = call(btrue, to_string);

    assert(strcmp(call(sfalse, to_cstring), "false") == 0);
    assert(strcmp(call(strue, to_cstring), "true") == 0);

    REFCDEC(bfalse);
    REFCDEC(btrue);
    REFCDEC(sfalse);
    REFCDEC(strue);
}

static void boolean_equals(void)
{
    Boolean * bfalse = new(Boolean, false);
    Boolean * btrue = new(Boolean, true);
    Boolean * b = new(Boolean, true);

    assert(call(b, equals, btrue));
    assert(call(btrue, equals, b));
    assert(call(btrue, equals, btrue));
    assert(!(call(b, equals, bfalse)));
    assert(!(call(btrue, equals, bfalse)));
    assert(!(call(btrue, equals, NULL)));
    assert(!(call(bfalse, equals, NULL)));

    Object * o = new(Object);
    String * s = new(String, "false");

    assert(!(call(o, equals, b)));
    assert(!(call(b, equals, o)));
    assert(!(call(s, equals, b)));
    assert(!(call(b, equals, s)));
    assert(!(call(bfalse, equals, s)));

    REFCDEC(o);
    REFCDEC(s);
    REFCDEC(b);
    REFCDEC(bfalse);
    REFCDEC(btrue);
}

static void boolean_from_string(void)
{
    Boolean * b = new(Boolean);
    String * s = new (String, "true");
    bool ok = call(b, from_string, s);
    assert(ok);
    assert(b->value);

    call(s, set_text, "false");
    ok = call(b, from_string, s);
    assert(ok);
    assert(!(b->value));

    call(s, set_text, "truee");
    ok = call(b, from_string, s);
    assert(!(ok));
    assert(!(b->value));

    REFCDEC(s);
    REFCDEC(b);
}

static void integer_add(void)
{
    Integer * i = new(Integer, 5);
    assert(type_equal(i, "Integer"));
    assert(i != NULL);
    assert((i->value) == (5));

    Integer * res = call(i, add_int, 4);
    assert(res != NULL);
    assert((res->value) == (9));

    Integer * j = new(Integer, 7);
    REFCDEC(res);
    res = call(i, add, j);
    assert((res->value) == (12));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void integer_increment(void)
{
    Integer * i = new(Integer, 5);
    Integer * j = new(Integer, 7);

    call(i, increment_int, 4);
    assert((i->value) == (9));
    call(i, increment, j);
    assert((i->value) == (16));

    REFCDEC(i);
    REFCDEC(j);
}

static void integer_subtract(void)
{
    Integer * i = new(Integer, 5);
    assert(i != NULL);
    assert((i->value) == (5));

    Integer * res = call(i, subtract_int, 4);
    assert(res != NULL);
    assert((res->value) == (1));

    Integer * j = new(Integer, 7);
    REFCDEC(res);
    res = call(i, subtract, j);
    assert((res->value) == (-2));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void integer_decrement(void)
{
    Integer * i = new(Integer, 5);
    Integer * j = new(Integer, 7);

    call(i, increment_int, 4);
    assert((i->value) == (9));
    call(i, increment, j);
    assert((i->value) == (16));

    REFCDEC(i);
    REFCDEC(j);
}

static void integer_multiply(void)
{
    Integer * i = new(Integer, 5);
    assert(i != NULL);
    assert((i->value) == (5));

    Integer * res = call(i, multiply_int, 4);
    assert(res != NULL);
    assert((res->value) == (20));

    Integer * j = new(Integer, 7);
    REFCDEC(res);
    res = call(i, multiply, j);
    assert((res->value) == (35));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void integer_multiply_inplace(void)
{
    Integer * i = new(Integer, 5);
    Integer * j = new(Integer, 7);

    call(i, multiply_inplace_int, 4);
    assert((i->value) == (20));
    call(i, multiply_inplace, j);
    assert((i->value) == (140));

    REFCDEC(i);
    REFCDEC(j);
}

static void integer_divide(void)
{
    Integer * i = new(Integer, 25);
    assert(i != NULL);
    assert((i->value) == (25));

    Integer * res = call(i, divide_int, 5);
    assert(res != NULL);
    assert((res->value) == (5));

    Integer * j = new(Integer, 10);
    REFCDEC(res);
    res = call(i, divide, j);
    assert((res->value) == (2));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void integer_divide_inplace(void)
{
    Integer * i = new(Integer, 50);
    Integer * j = new(Integer, -10);

    call(i, divide_inplace_int, 5);
    assert((i->value) == (10));
    call(i, divide_inplace, j);
    assert((i->value) == (-1));

    REFCDEC(i);
    REFCDEC(j);
}

static void integer_equals(void)
{
    Integer * i = new(Integer, 1);
    Integer * j = new(Integer, -1);
    Integer * k = new(Integer, 1);

    assert(call(i, equals, i));
    assert(call(i, equals, k));
    assert(call(k, equals, i));
    assert(!(call(i, equals, j)));
    assert(!(call(j, equals, i)));
    assert(!(call(i, equals, NULL)));

    Object * o = new(Object);
    String * s = new(String);
    assert(!(call(i, equals, o)));
    assert(!(call(o, equals, i)));
    assert(!(call(i, equals, s)));
    assert(!(call(s, equals, i)));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(k);
    REFCDEC(s);
    REFCDEC(o);
}

static void integer_to_string(void)
{
    Integer * i = new(Integer, -1234);
    String * s = call(i, to_string);
    assert(strcmp(call(s,to_cstring), "-1234") == 0);

    REFCDEC(i);
    REFCDEC(s);
}

static void integer_from_string(void)
{
    Integer * i = new(Integer);
    String * s = new(String, "123");

    bool ok = call(i, from_string, s);
    assert(ok);
    assert((i->value) == (123));

    call(s, set_text, "-1234");
    ok = call(i, from_string, s);
    assert(ok);
    assert((i->value) == (-1234));

    call(s, set_text, "abc");
    ok = call(i, from_string, s);
    assert(!(ok));
    assert((i->value) == (-1234));


    REFCDEC(s);
    REFCDEC(i);
}

static void long_add(void)
{
    Long * i = new(Long, 5L);
    assert(type_equal(i, "Long"));
    assert(i != NULL);
    assert((i->value) == (5L));

    Long * res = call(i, add_long, 4L);
    assert(res != NULL);
    assert((res->value) == (9L));

    Long * j = new(Long, 7L);
    REFCDEC(res);
    res = call(i, add, j);
    assert((res->value) == (12L));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void long_increment(void)
{
    Long * i = new(Long, 5L);
    Long * j = new(Long, 7L);

    call(i, increment_long, 4L);
    assert((i->value) == (9L));
    call(i, increment, j);
    assert((i->value) == (16L));

    REFCDEC(i);
    REFCDEC(j);
}

static void long_subtract(void)
{
    Long * i = new(Long, 5L);
    assert(i != NULL);
    assert((i->value) == (5L));

    Long * res = call(i, subtract_long, 4L);
    assert(res != NULL);
    assert((res->value) == (1L));

    Long * j = new(Long, 7L);
    REFCDEC(res);
    res = call(i, subtract, j);
    assert((res->value) == (-2L));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void long_decrement(void)
{
    Long * i = new(Long, 5L);
    Long * j = new(Long, 7L);

    call(i, increment_long, 4L);
    assert((i->value) == (9L));
    call(i, increment, j);
    assert((i->value) == (16L));

    REFCDEC(i);
    REFCDEC(j);
}

static void long_multiply(void)
{
    Long * i = new(Long, 5L);
    assert(i != NULL);
    assert((i->value) == (5L));

    Long * res = call(i, multiply_long, 4L);
    assert(res != NULL);
    assert((res->value) == (20L));

    Long * j = new(Long, 7L);
    REFCDEC(res);
    res = call(i, multiply, j);
    assert((res->value) == (35L));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void long_multiply_inplace(void)
{
    Long * i = new(Long, 5L);
    Long * j = new(Long, 7L);

    call(i, multiply_inplace_long, 4L);
    assert((i->value) == (20L));
    call(i, multiply_inplace, j);
    assert((i->value) == (140L));

    REFCDEC(i);
    REFCDEC(j);
}

static void long_divide(void)
{
    Long * i = new(Long, 25L);
    assert(i != NULL);
    assert((i->value) == (25L));

    Long * res = call(i, divide_long, 5L);
    assert(res != NULL);
    assert((res->value) == (5L));

    Long * j = new(Long, 10L);
    REFCDEC(res);
    res = call(i, divide, j);
    assert((res->value) == (2L));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void long_divide_inplace(void)
{
    Long * i = new(Long, 50L);
    Long * j = new(Long, -10L);

    call(i, divide_inplace_long, 5L);
    assert((i->value) == (10L));
    call(i, divide_inplace, j);
    assert((i->value) == (-1L));

    REFCDEC(i);
    REFCDEC(j);
}

static void long_equals(void)
{
    Long * i = new(Long, 1L);
    Long * j = new(Long, -1L);
    Long * k = new(Long, 1L);

    assert(call(i, equals, i));
    assert(call(i, equals, k));
    assert(call(k, equals, i));
    assert(!(call(i, equals, j)));
    assert(!(call(j, equals, i)));
    assert(!(call(i, equals, NULL)));

    Object * o = new(Object);
    String * s = new(String);
    assert(!(call(i, equals, o)));
    assert(!(call(o, equals, i)));
    assert(!(call(i, equals, s)));
    assert(!(call(s, equals, i)));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(k);
    REFCDEC(s);
    REFCDEC(o);
}

static void long_to_string(void)
{
    Long * i = new(Long, -1234L);
    String * s = call(i, to_string);
    assert(strcmp(call(s,to_cstring), "-1234") == 0);

    REFCDEC(i);
    REFCDEC(s);
}

static void long_from_string(void)
{
    Long * i = new(Long);
    String * s = new(String, "123");

    bool ok = call(i, from_string, s);
    assert(ok);
    assert((i->value) == (123L));

    call(s, set_text, "-1234");
    ok = call(i, from_string, s);
    assert(ok);
    assert((i->value) == (-1234L));

    call(s, set_text, "abc");
    ok = call(i, from_string, s);
    assert(!(ok));
    assert((i->value) == (-1234L));


    REFCDEC(s);
    REFCDEC(i);
}

static void double_add(void)
{
    Double * i = new(Double, 5.5);
    assert(type_equal(i, "Double"));
    assert(i != NULL);
    assert(fabs((i->value) - (5.5)) <= 0.000000001);

    Double * res = call(i, add_double, 4.0);
    assert(res != NULL);
    assert(fabs((res->value) - (9.5)) <= 0.000000001);

    Double * j = new(Double, 7.34);
    REFCDEC(res);
    res = call(i, add, j);
    assert(fabs((res->value) - (12.84)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void double_increment(void)
{
    Double * i = new(Double, 5.1);
    Double * j = new(Double, 7.2);

    call(i, increment_double, 4.2);
    assert(fabs((i->value) - (9.3)) <= 0.000000001);
    call(i, increment, j);
    assert(fabs((i->value) - (16.5)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
}

static void double_subtract(void)
{
    Double * i = new(Double, 5.1);
    assert(i != NULL);
    assert(fabs((i->value) - (5.1)) <= 0.000000001);

    Double * res = call(i, subtract_double, 4.2);
    assert(res != NULL);
    assert(fabs((res->value) - (0.9)) <= 0.000000001);

    Double * j = new(Double, 7.3);
    REFCDEC(res);
    res = call(i, subtract, j);
    assert(fabs((res->value) - (-2.2)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void double_decrement(void)
{
    Double * i = new(Double, 5.6);
    Double * j = new(Double, 0.2);

    call(i, increment_double, 1.5);
    assert(fabs((i->value) - (7.1)) <= 0.000000001);
    call(i, increment, j);
    assert(fabs((i->value) - (7.3)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
}

static void double_multiply(void)
{
    Double * i = new(Double, 5);
    assert(i != NULL);
    assert(fabs((i->value) - (5.0)) <= 0.000000001);

    Double * res = call(i, multiply_double, 4.1);
    assert(res != NULL);
    assert(fabs((res->value) - (20.5)) <= 0.000000001);

    Double * j = new(Double, 0.8);
    REFCDEC(res);
    res = call(i, multiply, j);
    assert(fabs((res->value) - (4)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void double_multiply_inplace(void)
{
    Double * i = new(Double, 4);
    Double * j = new(Double, 7.5);

    call(i, multiply_inplace_double, 4L);
    assert(fabs((i->value) - (16)) <= 0.000000001);
    call(i, multiply_inplace, j);
    assert(fabs((i->value) - (120)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
}

static void double_divide(void)
{
    Double * i = new(Double, 25.1);
    assert(i != NULL);
    assert(fabs((i->value) - (25.1)) <= 0.000000001);

    Double * res = call(i, divide_double, 5);
    assert(res != NULL);
    assert(fabs((res->value) - (5.02)) <= 0.000000001);

    Double * j = new(Double, 10);
    REFCDEC(res);
    res = call(i, divide, j);
    assert(fabs((res->value) - (2.51)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(res);
}

static void double_divide_inplace(void)
{
    Double * i = new(Double, 50.5);
    Double * j = new(Double, -10.1);

    call(i, divide_inplace_double, 5);
    assert(fabs((i->value) - (10.1)) <= 0.000000001);
    call(i, divide_inplace, j);
    assert(fabs((i->value) - (-1)) <= 0.000000001);

    REFCDEC(i);
    REFCDEC(j);
}

static void double_equals(void)
{
    Double * i = new(Double, 1.23);
    Double * j = new(Double, -1.23);
    Double * k = new(Double, 1.23);

    assert(call(i, equals, i));
    assert(call(i, equals, k));
    assert(call(k, equals, i));
    assert(!(call(i, equals, j)));
    assert(!(call(j, equals, i)));
    assert(!(call(i, equals, NULL)));

    Object * o = new(Object);
    String * s = new(String);
    assert(!(call(i, equals, o)));
    assert(!(call(o, equals, i)));
    assert(!(call(i, equals, s)));
    assert(!(call(s, equals, i)));

    REFCDEC(i);
    REFCDEC(j);
    REFCDEC(k);
    REFCDEC(s);
    REFCDEC(o);
}

static void double_to_string(void)
{
    Double * i = new(Double, -12.34);
    String * s = call(i, to_string);
    assert(strcmp(call(s,to_cstring), "-12.3400") == 0);

    REFCDEC(i);
    REFCDEC(s);
}

static void double_from_string(void)
{
    Double * i = new(Double);
    String * s = new(String, "1.23");

    bool ok = call(i, from_string, s);
    assert(ok);
    assert(fabs((i->value) - (1.23)) <= 0.000000001);

    call(s, set_text, "-12.34");
    ok = call(i, from_string, s);
    assert(ok);
    assert(fabs((i->value) - (-12.34)) <= 0.000000001);

    call(s, set_text, "abc");
    ok = call(i, from_string, s);
    assert(!(ok));
    assert(fabs((i->value) - (-12.34)) <= 0.000000001);


    REFCDEC(s);
    REFCDEC(i);
}
int main(void)
{
    boolean_new();
    boolean_equals();
    boolean_to_string();
    boolean_from_string();
    integer_add();
    integer_increment();
    integer_subtract();
    integer_decrement();
    integer_multiply();
    integer_multiply_inplace();
    integer_divide();
    integer_divide_inplace();
    integer_equals();
    integer_to_string();
    integer_from_string();
    long_add();
    long_increment();
    long_subtract();
    long_decrement();
    long_multiply();
    long_multiply_inplace();
    long_divide();
    long_divide_inplace();
    long_equals();
    long_to_string();
    long_from_string();
    double_add();
    double_increment();
    double_subtract();
    double_decrement();
    double_multiply();
    double_multiply_inplace();
    double_divide();
    double_divide_inplace();
    double_equals();
    double_to_string();
    double_from_string();
    return 0;
}


