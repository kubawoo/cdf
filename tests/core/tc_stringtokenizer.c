#include "stringtokenizer.h"
#include <assert.h>


static void string_split_by_char(void)
{
    String * s = new(String, "a,b,c");
    StringTokenizer * st = new(StringTokenizer, s);
    REFCDEC(s);
    List* l = call(st, split_by_char, ',');
    REFCDEC(st);
    assert((l->length) == (3));

    String * e = call(l, get, 0);
    assert(strcmp(call(e, to_cstring), "a") == 0);
    REFCDEC(e);

    e = call(l, get, 1);
    assert(strcmp(call(e, to_cstring), "b") == 0);
    REFCDEC(e);

    e = call(l, get, 2);
    assert(strcmp(call(e, to_cstring), "c") == 0);
    REFCDEC(e);

    REFCDEC(l);
}

static void string_split_by_char2(void)
{
    String * s = new(String, ",,,,a,b,,c,");
    StringTokenizer * st = new(StringTokenizer, s);
    REFCDEC(s);
    List* l = call(st, split_by_char, ',');
    REFCDEC(st);
    assert((l->length) == (3));

    String * e = call(l, get, 0);
    assert(strcmp(call(e, to_cstring), "a") == 0);
    REFCDEC(e);

    e = call(l, get, 1);
    assert(strcmp(call(e, to_cstring), "b") == 0);
    REFCDEC(e);

    e = call(l, get, 2);
    assert(strcmp(call(e, to_cstring), "c") == 0);
    REFCDEC(e);

    REFCDEC(l);
}
int main(void)
{
    string_split_by_char();
    string_split_by_char2();
    return 0;
}


