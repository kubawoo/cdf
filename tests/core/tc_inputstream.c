#include "inputstream.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void inputstream_new(void)
{
    InputStream *is = new(InputStream);
    assert(type_equal(is, "InputStream"));
    REFCDEC(is);
}

static void inputstream_read(void)
{
    InputStream *is = new(InputStream);
    int result = call(is, read);
    assert((result) == (-1)); // Base implementation should return -1
    REFCDEC(is);
}
int main(void)
{
    inputstream_new();
    inputstream_read();
    return 0;
}
