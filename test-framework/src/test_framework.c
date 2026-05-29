#define _GNU_SOURCE

#include "test_framework.h"
#include <unistd.h>
#include <time.h>

//http://stackoverflow.com/questions/11900904/wrong-numbers-with-clock-gettime-and-clock-process-cputime-id
#define NANOS_PER_MS 1000000LL
#define MS_PER_SEC 1000LL
#define USED_CLOCK CLOCK_MONOTONIC_RAW

double tick_ms() {
	struct timespec t;
	if(clock_gettime(USED_CLOCK, &t)) {
		return 0.0;
	}

	return (double) t.tv_sec * (double) MS_PER_SEC + (double) t.tv_nsec / (double) NANOS_PER_MS;
}

TestSuite * newTestSuite(const char * name) {
    TestSuite * s = malloc(sizeof(TestSuite));
    s->cases = NULL;
    s->count = 0;
    s->name = name;
    return s;
}


void _format_msg(TestResult * _tr, const char * file, int line, const char * fmt, ...) {
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    int prefix_len = snprintf(NULL, 0, "[%s:%d] ", file, line);
    int msg_len = vsnprintf(NULL, 0, fmt, ap);
    _tr->msg = malloc(prefix_len + msg_len + 1);
    if (_tr->msg) {
        sprintf(_tr->msg, "[%s:%d] ", file, line);
        vsprintf(_tr->msg + prefix_len, fmt, ap2);
    }
    va_end(ap2);
    va_end(ap);
}

void addTestCase(TestSuite * suite, const char * name, void (* f)(TEST_CASE_ARGUMENTS_RAW)) {
    suite->count++;
    TestCase ** new_cases = realloc(suite->cases, suite->count * sizeof(TestCase *));
    if (!new_cases) return;
    suite->cases = new_cases;
    TestCase * c = malloc(sizeof(TestCase));
    c->f = f;
    c->result = malloc(sizeof(TestResult));
    c->result->msg = NULL;
    c->result->success = true;
    c->name = name;
    suite->cases[suite->count - 1] = c;
}

