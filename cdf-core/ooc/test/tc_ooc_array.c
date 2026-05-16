#include "test_framework.h"
#include "ooc.h"
#include "ooc_array.h"

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


TEST_CASES_BEGIN
    TEST_CASE(array_test);
TEST_CASES_END

