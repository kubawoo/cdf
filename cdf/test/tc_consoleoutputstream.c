#include "consoleoutputstream.h"
#include "test_framework.h"

void consoleos_test(TEST_CASE_ARGUMENTS) {
    OutputStream * c = new(ConsoleOutputStream);
    String * s = new(String, "hello console output stream!\n");
    call(c, write_string, s);

    REFCDEC(s);
    REFCDEC(c);
}

TEST_CASES_BEGIN
    TEST_CASE(consoleos_test);
TEST_CASES_END

