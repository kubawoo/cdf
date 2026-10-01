#include "queue.h"


void Queue_delete(ObjectPtr _this) {
    make_this(Queue, _this);
    REFCDEC(this->_list);

    super_delete(Collection, _this);
}

static void Queue_enqueue(ObjectPtr _this, ObjectPtr e) {
    make_this(Queue, _this);
    call(this->_list, add, e);
}

static ObjectPtr Queue_dequeue(ObjectPtr _this) {
    make_this(Queue, _this);
    if (call(this->_list, size) == 0) {
        return nullptr;
    }
    ObjectPtr e = call(this->_list, get, 0);
    call(this->_list, remove, 0);
    return e;
}

static ObjectPtr Queue_peek(ObjectPtr _this) {
    make_this(Queue, _this);
    if (call(this->_list, size) == 0) {
        return nullptr;
    }
    return call(this->_list, get, 0);
}

static unsigned int Queue_size(ObjectPtr _this) {
    make_this(Queue, _this);
    return call(this->_list, size);
}

static String * Queue_to_string(ObjectPtr _this) {
	make_this(Queue, _this);
	return call(this->_list, to_string);
}

static Iterator * _iterator(ObjectPtr _this) {
    make_this(Queue, _this);
    return new(QueueIterator, this->_list);
}

Queue * Queue_new(Queue * this) {
    super(Collection, Queue);
    override(Object, to_string, Queue_to_string);
    override(Collection, iterator, _iterator);
    override(Collection, size, Queue_size);
    this->_list = new(List);
    this->enqueue = Queue_enqueue;
    this->dequeue = Queue_dequeue;
    this->peek = Queue_peek;
    return this;
}

static ObjectPtr _next(ObjectPtr _this) {
    make_this(QueueIterator, _this);
    if(!call(this, hasNext)) {
        return NULL;
    }
    return call(this->_list, get, this->_index++);
}

static bool _hasNext(ObjectPtr _this) {
    make_this(QueueIterator, _this);
    return this->_index < call(this->_list, size);
}

QueueIterator * QueueIterator_new1(QueueIterator * this, List * list) {
    super(Iterator, QueueIterator);
    override(Iterator, hasNext, _hasNext);
    override(Iterator, next, _next);
    REFCINC(list);
    this->_list = list;
    this->_index = 0;
    return this;
}

void QueueIterator_delete(ObjectPtr _this) {
    make_this(QueueIterator, _this);
    REFCDEC(this->_list);
    super_delete(Iterator, _this);
}
