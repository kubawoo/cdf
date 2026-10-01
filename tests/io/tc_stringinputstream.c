#include "stringinputstream.h"
#include <assert.h>


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
// A byte above 0x7F must read back as its unsigned value, not as a negative
// one: -1 is the end-of-stream marker, so a sign-extended byte was read as the
// end of the text and truncated any UTF-8 payload.
static void stringinputstream_reads_high_bytes(void)
{
    String * s = new(String, "a\xc3\xa9\xe4\xbd\xa0z");
    InputStream * is = new(StringInputStream, s);

    int expected[] = { 'a', 0xc3, 0xa9, 0xe4, 0xbd, 0xa0, 'z' };
    for(unsigned i = 0; i < sizeof expected / sizeof expected[0]; ++i) {
        assert(call(is, read) == expected[i]);
    }
    assert(call(is, read) == -1);

    REFCDEC(is);
    REFCDEC(s);
}

int main(void)
{
    stringinputstream_test();
    stringinputstream_reads_high_bytes();
    return 0;
}


