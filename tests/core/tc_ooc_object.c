#include "ooc_object.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void object_equals(void)
{
    Object * o1 = new(Object);
    assert(type_equal(o1, "Object"));
    Object * o2 = new(Object);

    assert(call(o1, equals, o1));
    assert(call(o2, equals, o2));
    assert(!(call(o2, equals, o1)));
    assert(!(call(o1, equals, o2)));
    assert(!(call(o1, equals, NULL)));

    delete(o1);
    delete(o2);
}

static void object_copy(void)
{
    Object * o = new(Object);
    Object * copy = call(o, copy);
    assert(copy == NULL);

    delete(o);
}

static void object_refc(void)
{
    Object * o = new(Object);
    assert((o->_refc) == (1));
    REFCINC(o);
    assert((o->_refc) == (2));
    REFCDEC(o);
    assert((o->_refc) == (1));
    REFCDEC(o);
    assert(o == NULL);

    o = REFCTMP(new(Object));
    assert(o != NULL);
    assert((o->_refc) == (0));
    REFCINC(o);
    assert((o->_refc) == (1));
    REFCDEC(o);
    assert(o == NULL);

    o = REFCTMP(new(Object));
    assert(o != NULL);
    assert((o->_refc) == (0));
    delete(o);
    assert(o == NULL);
}
int main(void)
{
    object_equals();
    object_copy();
    object_refc();
    return 0;
}

