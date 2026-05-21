#include "ooc_pair.h"
#include "ooc_primitives.h"
#include "test_framework.h"

void pair_test(TEST_CASE_ARGUMENTS) {
    String * s = REFCTMP(new(String, "hello"));
    Integer * i = REFCTMP(new(Integer, 5));
    Pair * pair = new(Pair, s, i);

    String * left = call(pair, left);
    Integer * right = call(pair, right);

    ASSERT_STRINGS_EQUAL(call(left, to_cstring), "hello");
    ASSERT_EQUAL(right->value, 5);

    String * pair_string = call(pair, to_string);
    ASSERT_STRINGS_EQUAL(call(pair_string, to_cstring), "<hello|5>");

    REFCDEC(pair_string);
    REFCDEC(left);
    REFCDEC(right);
    REFCDEC(pair);
}


TEST_CASES_BEGIN
    TEST_CASE(pair_test);
TEST_CASES_END

