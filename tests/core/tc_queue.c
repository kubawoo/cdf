#include "queue.h"
#include <assert.h>


static void queue_test(void)
{
    Queue * queue = new(Queue);
    assert(type_equal(queue, "Queue"));
    assert(queue->_list != NULL);
    assert((0) == (call(queue, size)));

    String * s = new(String, "hello");
    call(queue, enqueue, s);
    REFCDEC(s);

    assert((1) == (call(queue, size)));

    String * s2 = new(String, "world");
    call(queue, enqueue, s2);
    call(queue, enqueue, s2);
    REFCDEC(s2);
    assert((3) == (call(queue, size)));

    String * st = call(queue, to_string);
    assert(strcmp("[hello; world; world]", call(st, to_cstring)) == 0);
    REFCDEC(st);

    String * s3 = call(queue, peek);
    assert((3) == (call(queue, size)));
    assert(strcmp("hello", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    s3 = call(queue, dequeue);
    assert((2) == (call(queue, size)));
    assert(strcmp("hello", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    s3 = call(queue, dequeue);
    assert((1) == (call(queue, size)));
    assert(strcmp("world", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    s3 = call(queue, dequeue);
    assert((0) == (call(queue, size)));
    assert(strcmp("world", call(s3, to_cstring)) == 0);
    REFCDEC(s3);

    assert(call(queue, dequeue) == NULL);
    assert(call(queue, peek) == NULL);

    REFCDEC(queue);
}

static void queue_iterator_test(void)
{
    Queue * queue = new(Queue);
    call(queue, enqueue, REFCTMP(new(String, "first")));
    call(queue, enqueue, REFCTMP(new(String, "second")));
    call(queue, enqueue, REFCTMP(new(String, "third")));

    Iterator * it = call(queue, iterator);
    assert(type_equal(it, "QueueIterator"));

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
    REFCDEC(queue);
}

static void queue_iterator_empty(void)
{
    Queue * queue = new(Queue);
    Iterator * it = call(queue, iterator);
    assert(type_equal(it, "QueueIterator"));
    assert(!(call(it, hasNext)));
    assert(call(it, next) == NULL);
    REFCDEC(it);
    REFCDEC(queue);
}
int main(void)
{
    queue_test();
    queue_iterator_test();
    queue_iterator_empty();
    return 0;
}

