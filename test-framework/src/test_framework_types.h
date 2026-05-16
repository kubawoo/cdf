#ifndef CDF_TEST_FRAMEWORK_TYPES_H
#define CDF_TEST_FRAMEWORK_TYPES_H

#include <stdbool.h>

#define TEST_CASE_ARGUMENTS_RAW TestResult *
#define TEST_CASE_ARGUMENTS TestResult * _tr


typedef struct {
    bool success;
    char * msg;
} TestResult;

typedef struct {
    const char * name;
    void (*f)(TEST_CASE_ARGUMENTS_RAW);
    TestResult * result;
} TestCase;

typedef struct {
    const char * name;
    TestCase ** cases;
    unsigned int count;
} TestSuite;

#endif

