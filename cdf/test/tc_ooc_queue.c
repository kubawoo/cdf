#include "ooc.h"
#include "ooc_queue.h"
#include "test_framework.h"

void queue_test(TEST_CASE_ARGUMENTS) {
    Queue * queue = new(Queue);
    ASSERT_TRUE(type_equal(queue, "Queue"));
    ASSERT_NOT_NULL(queue->_list);
    ASSERT_EQUAL(0, call(queue, size));

    String * s = new(String, "hello");
    call(queue, enqueue, s);
    REFCDEC(s);

    ASSERT_EQUAL(1, call(queue, size));

    String * s2 = new(String, "world");
    call(queue, enqueue, s2);
    call(queue, enqueue, s2);
    REFCDEC(s2);
    ASSERT_EQUAL(3, call(queue, size));

    String * st = call(queue, to_string);
    ASSERT_STRINGS_EQUAL("[hello; world; world]", call(st, to_cstring));
    REFCDEC(st);

    String * s3 = call(queue, peek);
    ASSERT_EQUAL(3, call(queue, size));
    ASSERT_STRINGS_EQUAL("hello", call(s3, to_cstring));
    REFCDEC(s3);

    s3 = call(queue, dequeue);
    ASSERT_EQUAL(2, call(queue, size));
    ASSERT_STRINGS_EQUAL("hello", call(s3, to_cstring));
    REFCDEC(s3);

    s3 = call(queue, dequeue);
    ASSERT_EQUAL(1, call(queue, size));
    ASSERT_STRINGS_EQUAL("world", call(s3, to_cstring));
    REFCDEC(s3);

    s3 = call(queue, dequeue);
    ASSERT_EQUAL(0, call(queue, size));
    ASSERT_STRINGS_EQUAL("world", call(s3, to_cstring));
    REFCDEC(s3);

    ASSERT_NULL(call(queue, dequeue));
    ASSERT_NULL(call(queue, peek));

    REFCDEC(queue);
}

void queue_iterator_test(TEST_CASE_ARGUMENTS) {
    Queue * queue = new(Queue);
    call(queue, enqueue, REFCTMP(new(String, "first")));
    call(queue, enqueue, REFCTMP(new(String, "second")));
    call(queue, enqueue, REFCTMP(new(String, "third")));

    Iterator * it = call(queue, iterator);
    ASSERT_TRUE(type_equal(it, "QueueIterator"));

    String * s = call(it, next);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "first");
    REFCDEC(s);

    s = call(it, next);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "second");
    REFCDEC(s);

    s = call(it, next);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "third");
    REFCDEC(s);

    ASSERT_FALSE(call(it, hasNext));
    ASSERT_NULL(call(it, next));

    REFCDEC(it);
    REFCDEC(queue);
}

void queue_iterator_empty(TEST_CASE_ARGUMENTS) {
    Queue * queue = new(Queue);
    Iterator * it = call(queue, iterator);
    ASSERT_TRUE(type_equal(it, "QueueIterator"));
    ASSERT_FALSE(call(it, hasNext));
    ASSERT_NULL(call(it, next));
    REFCDEC(it);
    REFCDEC(queue);
}

TEST_CASES_BEGIN
    TEST_CASE(queue_test);
    TEST_CASE(queue_iterator_test);
    TEST_CASE(queue_iterator_empty);
TEST_CASES_END
