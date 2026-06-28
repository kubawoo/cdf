#include "log.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void testcase1(void)
{
    Logger * logger = new(Logger, REFCTMP(new(String, "test-logger")), LOG_LEVEL_DEBUG);
    String * msg = new(String, "my log message");
    call(logger, log, LOG_LEVEL_DEBUG, log_msg(msg));
    REFCDEC(logger);
    REFCDEC(msg);
}
int main(void)
{
    testcase1();
    return 0;
}


