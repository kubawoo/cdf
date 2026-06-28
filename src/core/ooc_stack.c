#include "ooc_stack.h"


void Stack_delete(ObjectPtr _this) {
    make_this(Stack, _this);
    REFCDEC(this->_list);

    super_delete(Collection, _this);
}

static void Stack_push(ObjectPtr _this, ObjectPtr e) {
    make_this(Stack, _this);
    call(this->_list, add, e);
}

static ObjectPtr Stack_pop(ObjectPtr _this) {
    make_this(Stack, _this);
    int pos = this->_list->length - 1;
    ObjectPtr e = call(this->_list, get, pos);
    call(this->_list, remove, pos);
    return e;
}

static ObjectPtr Stack_peek(ObjectPtr _this) {
    make_this(Stack, _this);
    int pos = this->_list->length - 1;
    return call(this->_list, get, pos);
}

static int Stack_size(ObjectPtr _this) {
    make_this(Stack, _this);
    return this->_list->length;
}

static String * Stack_to_string(ObjectPtr _this) {
	make_this(Stack, _this);
	return call(this->_list, to_string);
}

static Iterator * _iterator(ObjectPtr _this) {
    make_this(Stack, _this);
    return new(StackIterator, this->_list);
}

Stack * Stack_new(Stack * this) {
    super(Collection, Stack);
    override(Object, to_string, Stack_to_string);
    override(Collection, iterator, _iterator);
    this->_list = new(List);
    this->push = Stack_push;
    this->pop = Stack_pop;
    this->peek = Stack_peek;
    this->size = Stack_size;
    return this;
}

static ObjectPtr _next(ObjectPtr _this) {
    make_this(StackIterator, _this);
    if(!call(this, hasNext)) {
        return NULL;
    }
    return call(this->list, get, this->index++);
}

static bool _hasNext(ObjectPtr _this) {
    make_this(StackIterator, _this);
    return this->index < this->list->length;
}

StackIterator * StackIterator_new1(StackIterator * this, List * list) {
    super(Iterator, StackIterator);
    override(Iterator, hasNext, _hasNext);
    override(Iterator, next, _next);
    REFCINC(list);
    this->list = list;
    this->index = 0;
    return this;
}

void StackIterator_delete(ObjectPtr _this) {
    make_this(StackIterator, _this);
    REFCDEC(this->list);
    super_delete(Iterator, _this);
}
