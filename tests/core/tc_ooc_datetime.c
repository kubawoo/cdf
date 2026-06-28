#include "ooc_datetime.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void constructor_test(void)
{
    DateTime * dt = new(DateTime);
    assert(dt != NULL);
    assert(type_equal(dt, "DateTime"));
    String * s = call(dt, to_string);
    assert(s != NULL);

    delete(dt);
    delete(s);
}

static void to_string_test1(void)
{
    DateTime * dt = new(DateTime, 2001, 1, 1);
    String * s = call(dt, to_string);
    assert(strcmp(call(s, to_cstring), "2001-01-01T00:00:00") == 0);
    delete(s);
    delete(dt);
}

static void to_string_test2(void)
{
    DateTime * dt = new(DateTime, 2001, 12, 21, 14, 55, 3);
    String * s = call(dt, to_string);
    assert(strcmp(call(s, to_cstring), "2001-12-21T14:55:03") == 0);
    delete(s);
    delete(dt);
}

static void http_format(void)
{
    DateTime * dt = new(DateTime, 1994, 11, 15, 8, 12, 31);
    String * s = call(dt, format, "%a, %d %b %Y %H:%M:%S");
    assert(strcmp(call(s, to_cstring), "Tue, 15 Nov 1994 08:12:31") == 0);
    delete(s);
    delete(dt); 
}
int main(void)
{
    constructor_test();
    to_string_test1();
    to_string_test2();
    http_format();
    return 0;
}


