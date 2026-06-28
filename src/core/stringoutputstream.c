#include "stringoutputstream.h"

static void write(ObjectPtr _this, char c) {
    make_this(StringOutputStream, _this);
    call(this->_txt, append_char, c);
}

static String * to_string(ObjectPtr _this) {
	make_this(StringOutputStream, _this);
	REFCINC(this->_txt);
	return this->_txt;
}

StringOutputStream * StringOutputStream_new(StringOutputStream * this) {
    super(OutputStream, StringOutputStream);
    override(OutputStream, write, write);
    override(OutputStream, to_string, to_string);
    override(Object, to_string, to_string);
    this->_txt = new(String);
    return this;
}

void StringOutputStream_delete(ObjectPtr _this) {
    make_this(StringOutputStream, _this);
    REFCDEC(this->_txt);
    super_delete(OutputStream, _this);
}

