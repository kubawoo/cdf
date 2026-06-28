#include "stringinputstream.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void stringinputstream_test(void)
{
    String * s = new(String, "1a.");
    InputStream * is = new(StringInputStream, s);

    int c = call(is, read);
    assert((char)c == '1');
    c = call(is, read);
    assert((char)c == 'a');
    c = call(is, read);
    assert(c == '.');
    c = call(is, read);
    assert(c == -1);

    
    REFCDEC(is);
    REFCDEC(s);
}
int main(void)
{
    stringinputstream_test();
    return 0;
}


