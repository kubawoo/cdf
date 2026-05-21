#include "ooc_queue.h"


void Queue_delete(ObjectPtr _this) {
    make_this(Queue, _this);
    REFCDEC(this->_list);

    super_delete(Object, _this);
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

Queue * Queue_new(Queue * this) {
    super(Object, Queue);
    override(Object, to_string, Queue_to_string);
    this->_list = new(List);
    this->enqueue = Queue_enqueue;
    this->dequeue = Queue_dequeue;
    this->peek = Queue_peek;
    this->size = Queue_size;
    return this;
}
