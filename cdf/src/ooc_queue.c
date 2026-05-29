#include "ooc_queue.h"


void Queue_delete(ObjectPtr _this) {
    make_this(Queue, _this);
    REFCDEC(this->_list);

    super_delete(Collection, _this);
}

void Queue_enqueue(ObjectPtr _this, ObjectPtr e) {
    make_this(Queue, _this);
    call(this->_list, add, e);
}

ObjectPtr Queue_dequeue(ObjectPtr _this) {
    make_this(Queue, _this);
    if (this->_list->length == 0) {
        return NULL;
    }
    ObjectPtr e = call(this->_list, get, 0);
    call(this->_list, remove, 0);
    return e;
}

ObjectPtr Queue_peek(ObjectPtr _this) {
    make_this(Queue, _this);
    if (this->_list->length == 0) {
        return NULL;
    }
    return call(this->_list, get, 0);
}

int Queue_size(ObjectPtr _this) {
    make_this(Queue, _this);
    return this->_list->length;
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
    this->_list = new(List);
    this->enqueue = Queue_enqueue;
    this->dequeue = Queue_dequeue;
    this->peek = Queue_peek;
    this->size = Queue_size;
    return this;
}

static ObjectPtr _next(ObjectPtr _this) {
    make_this(QueueIterator, _this);
    if(!call(this, hasNext)) {
        return NULL;
    }
    return call(this->list, get, this->index++);
}

static bool _hasNext(ObjectPtr _this) {
    make_this(QueueIterator, _this);
    return this->index < this->list->length;
}

QueueIterator * QueueIterator_new1(QueueIterator * this, List * list) {
    super(Iterator, QueueIterator);
    override(Iterator, hasNext, _hasNext);
    override(Iterator, next, _next);
    REFCINC(list);
    this->list = list;
    this->index = 0;
    return this;
}

void QueueIterator_delete(ObjectPtr _this) {
    make_this(QueueIterator, _this);
    REFCDEC(this->list);
    super_delete(Iterator, _this);
}
