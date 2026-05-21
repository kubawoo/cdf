#include "inputstream.h"
#include "test_framework.h"

void inputstream_new(TEST_CASE_ARGUMENTS) {
    InputStream *is = new(InputStream);
    ASSERT_TRUE(type_equal(is, "InputStream"));
    REFCDEC(is);
}

void inputstream_read(TEST_CASE_ARGUMENTS) {
    InputStream *is = new(InputStream);
    int result = call(is, read);
    ASSERT_EQUAL(result, -1); // Base implementation should return -1
    REFCDEC(is);
}

TEST_CASES_BEGIN
    TEST_CASE(inputstream_new);
    TEST_CASE(inputstream_read);
TEST_CASES_END