#include "pair.h"


void Pair_delete(ObjectPtr _this) {
    make_this(Pair, _this);
    REFCDEC(this->_left);
    REFCDEC(this->_right);

    super_delete(Object, _this);
}

static ObjectPtr Pair_left(ObjectPtr _this) {
    make_this(Pair, _this);
    REFCINC(this->_left);
    return this->_left;
}

static ObjectPtr Pair_right(ObjectPtr _this) {
    make_this(Pair, _this);
    REFCINC(this->_right);
    return this->_right;
}


static String * Pair_to_string(ObjectPtr _this) {
	make_this(Pair, _this);
    String * left = call(this->_left, to_string);
    String * right = call(this->_right, to_string);
    String * s = new(String, "<");
    call(s, append, left);
    call(s, append_char, '|');
    call(s, append, right);
    call(s, append_char, '>');
    REFCDEC(left);
    REFCDEC(right);
	return s;
}

Pair * Pair_new2(Pair * this, ObjectPtr left, ObjectPtr right) {
    super(Object, Pair);
    override(Object, to_string, Pair_to_string);
    REFCINC(left);
    this->_left = left;
    REFCINC(right);
    this->_right = right;
    this->left = Pair_left;
    this->right = Pair_right;
    return this;
}

