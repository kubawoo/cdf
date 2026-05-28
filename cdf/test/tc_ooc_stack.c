#include "ooc.h"
#include "ooc_stack.h"
#include "test_framework.h"

void stack_test(TEST_CASE_ARGUMENTS) {
    Stack * stack = new(Stack);
    ASSERT_TRUE(type_equal(stack, "Stack"));
    ASSERT_NOT_NULL(stack ->_list);
    ASSERT_EQUAL(0, call(stack, size));

    String * s = new(String, "hello");
    call(stack, push, s);
    REFCDEC(s);

    ASSERT_EQUAL(1, call(stack, size));

    String * s2 = new(String, "world");
    call(stack, push, s2);
    call(stack, push, s2);
    REFCDEC(s2);
    ASSERT_EQUAL(3, call(stack, size));

    String * st = call(stack, to_string);
    ASSERT_STRINGS_EQUAL("[hello; world; world]", call(st, to_cstring));
    REFCDEC(st);

    String * s3 = call(stack, peek);
    ASSERT_EQUAL(3, call(stack, size));
    ASSERT_STRINGS_EQUAL("world", call(s3, to_cstring));
    REFCDEC(s3);

    s3 = call(stack, pop);
    ASSERT_EQUAL(2, call(stack, size));
    ASSERT_STRINGS_EQUAL("world", call(s3, to_cstring));
    REFCDEC(s3);

    s3 = call(stack, pop);
    ASSERT_EQUAL(1, call(stack, size));
    ASSERT_STRINGS_EQUAL("world", call(s3, to_cstring));
    REFCDEC(s3);

    s3 = call(stack, pop);
    ASSERT_EQUAL(0, call(stack, size));
    ASSERT_STRINGS_EQUAL("hello", call(s3, to_cstring));
    REFCDEC(s3);

    ASSERT_NULL(call(stack, pop));
    ASSERT_NULL(call(stack, peek));

    REFCDEC(stack);
}

void stack_iterator_test(TEST_CASE_ARGUMENTS) {
    Stack * stack = new(Stack);
    call(stack, push, REFCTMP(new(String, "first")));
    call(stack, push, REFCTMP(new(String, "second")));
    call(stack, push, REFCTMP(new(String, "third")));

    Iterator * it = call(stack, iterator);
    ASSERT_TRUE(type_equal(it, "StackIterator"));

    String * s = call(it, next);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "first");
    REFCDEC(s);

    s = call(it, next);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "second");
    REFCDEC(s);

    s = call(it, next);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "third");
    REFCDEC(s);

    ASSERT_FALSE(call(it, hasNext));
    ASSERT_NULL(call(it, next));

    REFCDEC(it);
    REFCDEC(stack);
}

void stack_iterator_empty(TEST_CASE_ARGUMENTS) {
    Stack * stack = new(Stack);
    Iterator * it = call(stack, iterator);
    ASSERT_TRUE(type_equal(it, "StackIterator"));
    ASSERT_FALSE(call(it, hasNext));
    ASSERT_NULL(call(it, next));
    REFCDEC(it);
    REFCDEC(stack);
}

TEST_CASES_BEGIN
    TEST_CASE(stack_test);
    TEST_CASE(stack_iterator_test);
    TEST_CASE(stack_iterator_empty);
TEST_CASES_END
