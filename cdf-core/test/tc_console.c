#include "test_framework.h"
#include "console.h"

void console_test(TEST_CASE_ARGUMENTS) {
    Console * c = new(Console);
    String * s = new(String, "hello console");
    Object * o = new(Object);
    call(c, print_object, c);
    call(c, print_object, s);
    call(c, print_object, o);

    REFCDEC(s);
    REFCDEC(o);
    REFCDEC(c);
}

TEST_CASES_BEGIN
    TEST_CASE(console_test);
TEST_CASES_END

