# `test-framework`

## Introduction

`test-framework` is a tool for writing and running unit tests.

## Building the `test-framework`


```
make
make test # optionally run unit tests
```

This will create a `dist` directory with the following subdirectories:
- `bin` contains the test runner binary
- `include` contains header files
- `lib` contains libs

## Writing a test case

The simplest possible test case looks as follows:


```
# (1) 
# file name: tc_hello_test.c

# (2)
#include "test_framework.h"

# (3)
void testcase(TEST_CASE_ARGUMENTS) {
	ASSERT_EQUAL(1, 1);
}

void testcase2(TEST_CASE_ARGUMENTS) {
	ASSERT_NOT_EQUAL(1, 2);
}

# (4)
TEST_CASES_BEGIN
    TEST_CASE(testcase);
    TEST_CASE(testcase2);
TEST_CASES_END
```

The most important parts of the test case are:
1. test file name should be prefixed with `tc_`
2. each test case needs to include `test_framework.h` header file
3. next, one defines a number of test cases to run; every test case needs to have `TEST_CASE_ARGUMENTS` as its parameters declaration and should not return any results
4. all tests cases should be listed between `TEST_CASES_BEGIN` and `TEST_CASE_END` and should be annotated with `TEST_CASE` 


### Assertions

Currenty the following assertins are provided by the framework:

- `ASSERT(condition, msg, vargs, ...)` - generic assert, fails if `condition` is evaluated as `false`; `msg` will be printed on assert failure and can use values from `vargs` 
- `ASSERT_TRUE(x)` - fails if `x` is `false`
- `ASSERT_FALSE(x)` - fails if `x` is `true`
- `ASSERT_STRINGS_EQUAL(x,y)` - compares two C-strings
- `ASSERT_NOT_NULL(x)` - fails if `x` is not a `null` pointer
- `ASSERT_NULL(x)` - fails if `x` is a `null` pointer
- `ASSERT_EQUAL(x,y)` - fails if two values are not equal
- `ASSERT_NOT_EQUAL(x,y)` - fails if two values are equal
- `ASSERT_ALMOST_EQUAL(x,y)` - compares two floating-point numbers
- `ASSERT_POINTERS_EQUAL(x,y)` - fails if two pointers point to different locations
- `ASSERT_POINTERS_NOT_EQUAL(x,y)` - fails if two pointers point to the same location
- `FAIL(msg)` - always fails with a message


### Building a test case

Assuming we want to compile a test case named `tc_hello_test.c` one should execute the following command:

```
gcc -Wall -fPIC -shared -I/path_to_test_framework/dist/include tc_hello_test.c -o tc_hello_test.so
```


### Running a test case

In order to run the tests, one shold use the following command:

```
LD_LIBRARY_PATH=".:/path_to_test_framework/dist/lib" /path_to_test_framework/dist/bin/testrunner tc_hello_test.so
```
