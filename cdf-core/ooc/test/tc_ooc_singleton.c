#include "test_framework.h"
#include "ooc.h"

void singleton_test(TEST_CASE_ARGUMENTS) {
	String * a = _cdf_get_singleton("String");
	ASSERT_EQUAL(a, NULL);
	String * b = _cdf_set_singleton("String", new(String));
	ASSERT_NOT_EQUAL(b, NULL);
}

void singleton_test2(TEST_CASE_ARGUMENTS) {
	Integer * i = singleton(Integer);
	Integer * j = singleton(Integer);
	Integer * k = singleton(Integer);

	ASSERT_NOT_NULL(i);
	ASSERT_NOT_NULL(j);
	ASSERT_NOT_NULL(k);
	ASSERT_POINTERS_EQUAL(i, j);
	ASSERT_POINTERS_EQUAL(i, k);
	ASSERT_POINTERS_EQUAL(j, k);
}

TEST_CASES_BEGIN
    TEST_CASE(singleton_test);
    TEST_CASE(singleton_test2);
TEST_CASES_END

