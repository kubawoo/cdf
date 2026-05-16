#include "test_framework.h"
#include "loggerfactory.h"

void testcase1(TEST_CASE_ARGUMENTS) {
	LoggerFactory * lf = singleton(LoggerFactory);
	String * logger_name = new(String, "my logger");
	Logger * logger = call(lf, get_logger, logger_name);
    String * msg = new(String, "my log message");
    call(logger, log, LOG_LEVEL_INFO, log_msg(msg));
    REFCDEC(logger);
    REFCDEC(logger_name);
    REFCDEC(msg);
}


TEST_CASES_BEGIN
    TEST_CASE(testcase1);
TEST_CASES_END

