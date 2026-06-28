#include "ooc.h"
#include "ooc_array.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void array_test(void)
{
    Array * a = new(Array, 10);
    assert(type_equal(a, "Array"));
    assert((a->length) == (10));
    assert(a->_values != NULL);
    for(int i = 0; i < 10; ++i) {
        assert(call(a, get, i) == NULL);
    }

    call(a, set, 3, REFCTMP(new(String, "one two free")));

    assert((a->length) == (10));
    assert(a->_values != NULL);
    for(int i = 0; i < 10; ++i) {
        Object * o = call(a, get, i);
        if(i == 3) {
            assert(o != NULL);
            assert(strcmp("String", o->type) == 0);
            assert(strcmp(call((String *) o, to_cstring), "one two free") == 0);
        } else {
            assert(o == NULL);
        }
        REFCDEC(o);
    }
    REFCDEC(a);
}

static void array_iterator_test(void)
{
    Array * a = new(Array, 5);
    call(a, set, 0, REFCTMP(new(String, "zero")));
    call(a, set, 1, REFCTMP(new(String, "one")));
    call(a, set, 2, REFCTMP(new(String, "two")));
    call(a, set, 3, REFCTMP(new(String, "three")));
    call(a, set, 4, REFCTMP(new(String, "four")));

    Iterator * it = call(a, iterator);
    assert(type_equal(it, "ArrayIterator"));

    const char *expected[] = {"zero", "one", "two", "three", "four"};
    for(int i = 0; i < 5; ++i) {
        assert(call(it, hasNext));
        String * s = call(it, next);
        assert(s != NULL);
        assert(strcmp(call(s, to_cstring), expected[i]) == 0);
        REFCDEC(s);
    }

    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);
    assert(call(it, next) == NULL);

    REFCDEC(it);
    REFCDEC(a);
}

static void array_iterator_empty(void)
{
    Array * a = new(Array, 3);
    Iterator * it = call(a, iterator);
    assert(type_equal(it, "ArrayIterator"));
    assert(call(it, hasNext));
    assert(call(it, next) == NULL);
    assert(call(it, hasNext));
    assert(call(it, next) == NULL);
    assert(call(it, hasNext));
    assert(call(it, next) == NULL);
    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);
    REFCDEC(it);
    REFCDEC(a);
}
int main(void)
{
    array_test();
    array_iterator_test();
    array_iterator_empty();
    return 0;
}

