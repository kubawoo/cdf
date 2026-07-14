#include "stack.h"
#include <assert.h>


static void stack_test(void)
{
    Stack * stack = new(Stack);
    assert(type_equal(stack, "Stack"));
    assert(stack ->_list != NULL);
    assert((0) == (call(stack, size)));

    String * s = new(String, "hello");
    call(stack, push, s);
    REFCDEC(s);

    assert((1) == (call(stack, size)));

    String * s2 = new(String, "world");
    call(stack, push, s2);
    call(stack, push, s2);
    REFCDEC(s2);
    assert((3) == (call(stack, size)));

    String * st = call(stack, to_string);
    assert(strcmp("[hello; world; world]", call(st, to_cstring)) == 0);
    REFCDEC(st);

    String * s3 = call(stack, peek);
    assert((3) == (call(stack, size)));
    assert(strcmp("world", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    s3 = call(stack, pop);
    assert((2) == (call(stack, size)));
    assert(strcmp("world", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    s3 = call(stack, pop);
    assert((1) == (call(stack, size)));
    assert(strcmp("world", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    s3 = call(stack, pop);
    assert((0) == (call(stack, size)));
    assert(strcmp("hello", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    assert(call(stack, pop) == NULL);
    assert(call(stack, peek) == NULL);

    REFCDEC(stack);
}

static void stack_iterator_test(void)
{
    Stack * stack = new(Stack);
    call(stack, push, REFCTMP(new(String, "first")));
    call(stack, push, REFCTMP(new(String, "second")));
    call(stack, push, REFCTMP(new(String, "third")));

    Iterator * it = call(stack, iterator);
    assert(type_equal(it, "StackIterator"));

    String * s = call(it, next);
    assert(strcmp(call(s, to_cstring), "first") == 0);
    REFCDEC(s);

    s = call(it, next);
    assert(strcmp(call(s, to_cstring), "second") == 0);
    REFCDEC(s);

    s = call(it, next);
    assert(strcmp(call(s, to_cstring), "third") == 0);
    REFCDEC(s);

    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);

    REFCDEC(it);
    REFCDEC(stack);
}

static void stack_iterator_empty(void)
{
    Stack * stack = new(Stack);
    Iterator * it = call(stack, iterator);
    assert(type_equal(it, "StackIterator"));
    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);
    REFCDEC(it);
    REFCDEC(stack);
}
int main(void)
{
    stack_test();
    stack_iterator_test();
    stack_iterator_empty();
    return 0;
}

