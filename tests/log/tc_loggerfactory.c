#include "loggerfactory.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void testcase1(void)
{
	LoggerFactory * lf = singleton(LoggerFactory);
	String * logger_name = new(String, "my logger");
	Logger * logger = call(lf, get_logger, logger_name);
    String * msg = new(String, "my log message");
    call(logger, log, LOG_LEVEL_INFO, log_msg(msg));
    REFCDEC(logger);
    REFCDEC(logger_name);
    REFCDEC(msg);
}
int main(void)
{
    testcase1();
    return 0;
}


