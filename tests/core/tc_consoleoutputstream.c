#include "consoleoutputstream.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void consoleos_test(void)
{
    OutputStream * c = new(ConsoleOutputStream);
    String * s = new(String, "hello console output stream!\n");
    call(c, write_string, s);

    REFCDEC(s);
    REFCDEC(c);
}
int main(void)
{
    consoleos_test();
    return 0;
}


