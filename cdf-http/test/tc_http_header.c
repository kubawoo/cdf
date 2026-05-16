#include "test_framework.h"
#include "http_types.h"

void to_string_test(TEST_CASE_ARGUMENTS) {
    HttpHeader * header = new(HttpHeader, REFCTMP(new(String, "Content-Type")), REFCTMP(new(String, "application/json")));
    String * s = call(header, to_string);
    ASSERT_STRINGS_EQUAL("Content-Type: application/json", call(s, to_cstring));
    delete(s);
    delete(header);
}


TEST_CASES_BEGIN
    TEST_CASE(to_string_test);
TEST_CASES_END

