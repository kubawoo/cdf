#include "test_framework.h"
#include "stringoutputstream.h"

void stringoutputstream_test(TEST_CASE_ARGUMENTS) {
    OutputStream * os = new(StringOutputStream);
    call(os, write, 'a');
    call(os, write, 'b');
    call(os, write, 'c');
    call(os, write, 'd');

    String * s = new(String, "efgh");
    call(os, write_string, s);
    REFCDEC(s);

    s = call(os, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "abcdefgh");

    
    REFCDEC(os);
    REFCDEC(s);
}

TEST_CASES_BEGIN
    TEST_CASE(stringoutputstream_test);
TEST_CASES_END

