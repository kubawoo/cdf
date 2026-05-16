#include "test_framework.h"

void testcase(TEST_CASE_ARGUMENTS) {
	ASSERT_EQUAL(1, 1);
	ASSERT_NOT_EQUAL(1, 2);
}


TEST_CASES_BEGIN
    TEST_CASE(testcase);
TEST_CASES_END

