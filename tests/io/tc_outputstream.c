#include "outputstream.h"
#include <assert.h>


static void outputstream_new(void)
{
    OutputStream *os = new(OutputStream);
    assert(type_equal(os, "OutputStream"));
    REFCDEC(os);
}

static void outputstream_write(void)
{
    OutputStream *os = new(OutputStream);
    // Base implementation should do nothing
    call(os, write, 'A');
    REFCDEC(os);
}

static void outputstream_write_string(void)
{
    OutputStream *os = new(OutputStream);
    String *s = new(String, "test");
    // Base implementation should do nothing
    call(os, write_string, s);
    REFCDEC(s);
    REFCDEC(os);
}

static void outputstream_to_string(void)
{
    OutputStream *os = new(OutputStream);
    String *result = call(os, to_string);
    // Should return NULL or empty string from base implementation
    // Actually, it calls super_method(Object, to_string) which would return the class name
    assert(result != NULL);
    REFCDEC(result);
    REFCDEC(os);
}
int main(void)
{
    outputstream_new();
    outputstream_write();
    outputstream_write_string();
    outputstream_to_string();
    return 0;
}
