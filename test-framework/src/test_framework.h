#ifndef CDF_TEST_FRAMEWORK_H
#define CDF_TEST_FRAMEWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "test_framework_types.h"


#define TEST_CASES_BEGIN TestSuite * init_test_suite(void) { TestSuite * suite = newTestSuite(__FILE__);
#define TEST_CASES_END return suite; }
#define TEST_CASE(name) addTestCase(suite, #name,  name)


void _format_msg(TestResult * _tr, const char * file, int line, const char * fmt, ...);
#define _MSG(_msg, vargs...) _format_msg(_tr, __FILE__, __LINE__, _msg, ##vargs)
#define ASSERT(x, _msg, vargs...) { if(!_tr->success) { return; }  if(!(x)) { _tr->success = false; _MSG(_msg, ##vargs); return; } }
#define ASSERT_TRUE(x) ASSERT(x, #x " is not true")
#define ASSERT_FALSE(x) ASSERT(!(x), #x " should be false")
#define ASSERT_STRINGS_EQUAL(x,y) ASSERT(strcmp(x,y) == 0, "Expected '%s' but got '%s'", y, x)
#define ASSERT_NOT_NULL(x) ASSERT(x != NULL, #x " is null")
#define ASSERT_NULL(x) ASSERT(x == NULL, #x " is not null")
#define ASSERT_EQUAL(x,y) ASSERT((x) == (y), #x " is not equal to " #y)
#define ASSERT_NOT_EQUAL(x,y) ASSERT((x) != (y), #x " should not be equal to " #y)
#define ASSERT_ALMOST_EQUAL(x,y) ASSERT(fabs((x) - (y) <= 0.000000001), #x " is not equal to " #y)
#define ASSERT_POINTERS_EQUAL(x,y) ASSERT((void *)(x) - (void *)(y) == 0, #x " is not equal to " #y)
#define ASSERT_POINTERS_NOT_EQUAL(x,y) ASSERT((void *)(x) - (void *)(y) != 0, #x " should not be equal to " #y)
#define FAIL(msg) ASSERT(0 != 0, msg)

TestSuite * newTestSuite(const char * name);
void addTestCase(TestSuite * suite, const char * name, void (* f)(TEST_CASE_ARGUMENTS_RAW));
double tick_ms();

#endif

