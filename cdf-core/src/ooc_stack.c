#include "ooc_stack.h"


void Stack_delete(ObjectPtr _this) {
    make_this(Stack, _this);
    REFCDEC(this->_list);

    super_delete(Object, _this);
}

void Stack_push(ObjectPtr _this, ObjectPtr e) {
    make_this(Stack, _this);
    call(this->_list, add, e);
}

ObjectPtr Stack_pop(ObjectPtr _this) {
    make_this(Stack, _this);
    int pos = this->_list->length - 1;
    ObjectPtr e = call(this->_list, get, pos);
    call(this->_list, remove, pos);
    return e;
}

ObjectPtr Stack_peek(ObjectPtr _this) {
    make_this(Stack, _this);
    int pos = this->_list->length - 1;
    return call(this->_list, get, pos);
}

int Stack_size(ObjectPtr _this) {
    make_this(Stack, _this);
    return this->_list->length;
}

static String * Stack_to_string(ObjectPtr _this) {
	make_this(Stack, _this);
	return call(this->_list, to_string);
}

Stack * Stack_new(Stack * this) {
    super(Object, Stack);
    override(Object, to_string, Stack_to_string);
    this->_list = new(List);
    this->push = Stack_push;
    this->pop = Stack_pop;
    this->peek = Stack_peek;
    this->size = Stack_size;
    return this;
}

