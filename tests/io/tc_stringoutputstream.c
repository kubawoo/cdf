#include "stringoutputstream.h"
#include <assert.h>


static void stringoutputstream_test(void)
{
    OutputStream * os = new(StringOutputStream);
    call(os, write, 'a');
    call(os, write, 'b');
    call(os, write, 'c');
    call(os, write, 'd');

    String * s = new(String, "efgh");
    call(os, write_string, s);
    REFCDEC(s);

    s = call(os, to_string);
    assert(strcmp(call(s, to_cstring), "abcdefgh") == 0);

    
    REFCDEC(os);
    REFCDEC(s);
}
int main(void)
{
    stringoutputstream_test();
    return 0;
}


