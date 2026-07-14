#include "pair.h"
#include "primitives.h"
#include <assert.h>


static void pair_test(void)
{
    String * s = REFCTMP(new(String, "hello"));
    Integer * i = REFCTMP(new(Integer, 5));
    Pair * pair = new(Pair, s, i);

    String * left = call(pair, left);
    Integer * right = call(pair, right);

    assert(strcmp(call(left, to_cstring), "hello") == 0);
    assert((right->value) == (5));

    String * pair_string = call(pair, to_string);
    assert(strcmp(call(pair_string, to_cstring), "<hello|5>") == 0);

    REFCDEC(pair_string);
    REFCDEC(left);
    REFCDEC(right);
    REFCDEC(pair);
}
int main(void)
{
    pair_test();
    return 0;
}


