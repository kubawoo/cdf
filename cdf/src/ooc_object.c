#include "ooc_object.h"
#include "ooc_string.h"

bool Object_equals(void * _this, void * _other) {
    return _this == _other;
}

String * _ooc_Object_to_string(ObjectPtr _this) {
    String * s = new(String, "Object of type ");
    call(s,  append_cstring, ((Object *)_this)->type);
    return s;
}

ObjectPtr Object_copy(ObjectPtr _this) {
    return NULL;
}

void Object_delete(ObjectPtr _this) {
}

Object * Object_new(Object * this) {
    if (!this) {
        this = pool_alloc(sizeof(Object));
    }
    this->_refc = 1;
    this->_ooc_destructor = Object_delete;
    this->type = "Object";
    this->to_string = _ooc_Object_to_string;
    this->equals = Object_equals;
    this->copy = Object_copy;
    return this;
}

void * _refctmp(void * o) {
    ((Object *) o)->_refc--;
    return o;
}

