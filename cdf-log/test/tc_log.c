#include "test_framework.h"
#include "log.h"

void testcase1(TEST_CASE_ARGUMENTS) {
    Logger * logger = new(Logger, REFCTMP(new(String, "test-logger")), LOG_LEVEL_DEBUG);
    String * msg = new(String, "my log message");
    call(logger, log, LOG_LEVEL_DEBUG, log_msg(msg));
    REFCDEC(logger);
    REFCDEC(msg);
}


TEST_CASES_BEGIN
    TEST_CASE(testcase1);
TEST_CASES_END

