#include "ooc_array.h"
#include <stdio.h>

void _check_index(Array * this, int index) {
    if(index < 0 || index >= this->length) {
         fputs("index out of bound", stderr);
         abort();
    }
}

void _set(ObjectPtr _this, int index, ObjectPtr o) {
    make_this(Array, _this);
    _check_index(this, index);
    REFCINC(o);
    this->_values[index] = o;
}


ObjectPtr _get(ObjectPtr _this, int index) {
    make_this(Array, _this);
    _check_index(this, index);
    ObjectPtr o = this->_values[index];
    REFCINC(o);
    return o;
}


Array * Array_new1(Array * this, int size) {
    if(size <= 0) {
        return NULL;
    }
    super(Object, Array);
    this->length = size;
    this->_values = malloc(size * sizeof(Object *));
    for(int i = 0; i < this->length; ++i) {
        this->_values[i] = NULL;
    }
    this->set = _set;
    this->get = _get;
    return this;
}

void Array_delete(ObjectPtr _this) {
    make_this(Array, _this);
    for(int i = 0; i < this->length; ++i) {
        REFCDEC(this->_values[i]);
    }
    free(this->_values);
    super_delete(Object, this);
}

