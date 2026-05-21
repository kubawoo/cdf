#include "stringinputstream.h"
#include "test_framework.h"

void stringinputstream_test(TEST_CASE_ARGUMENTS) {
    String * s = new(String, "1a.");
    InputStream * is = new(StringInputStream, s);

    int c = call(is, read);
    ASSERT_TRUE((char)c == '1');
    c = call(is, read);
    ASSERT_TRUE((char)c == 'a');
    c = call(is, read);
    ASSERT_TRUE(c == '.');
    c = call(is, read);
    ASSERT_TRUE(c == -1);

    
    REFCDEC(is);
    REFCDEC(s);
}

TEST_CASES_BEGIN
    TEST_CASE(stringinputstream_test);
TEST_CASES_END

