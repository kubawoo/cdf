#include "http_types.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void to_string_test(void)
{
    HttpHeader * header = new(HttpHeader, REFCTMP(new(String, "Content-Type")), REFCTMP(new(String, "application/json")));
    String * s = call(header, to_string);
    assert(strcmp("Content-Type: application/json", call(s, to_cstring)) == 0);
    REFCDEC(s);
    REFCDEC(header);
}
int main(void)
{
    to_string_test();
    return 0;
}


