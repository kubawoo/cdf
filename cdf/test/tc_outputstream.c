#include "ooc_string.h"
#include "outputstream.h"
#include "test_framework.h"

void outputstream_new(TEST_CASE_ARGUMENTS) {
    OutputStream *os = new(OutputStream);
    ASSERT_TRUE(type_equal(os, "OutputStream"));
    REFCDEC(os);
}

void outputstream_write(TEST_CASE_ARGUMENTS) {
    OutputStream *os = new(OutputStream);
    // Base implementation should do nothing
    call(os, write, 'A');
    REFCDEC(os);
}

void outputstream_write_string(TEST_CASE_ARGUMENTS) {
    OutputStream *os = new(OutputStream);
    String *s = new(String, "test");
    // Base implementation should do nothing
    call(os, write_string, s);
    REFCDEC(s);
    REFCDEC(os);
}

void outputstream_to_string(TEST_CASE_ARGUMENTS) {
    OutputStream *os = new(OutputStream);
    String *result = call(os, to_string);
    // Should return NULL or empty string from base implementation
    // Actually, it calls super_method(Object, to_string) which would return the class name
    ASSERT_NOT_NULL(result);
    REFCDEC(result);
    REFCDEC(os);
}

TEST_CASES_BEGIN
    TEST_CASE(outputstream_new);
    TEST_CASE(outputstream_write);
    TEST_CASE(outputstream_write_string);
    TEST_CASE(outputstream_to_string);
TEST_CASES_END