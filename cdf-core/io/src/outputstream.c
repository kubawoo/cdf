#include "outputstream.h"

static void write(ObjectPtr _this, char c) {
}

static void write_string(ObjectPtr _this, String * s) {
	make_this(OutputStream, _this);
	const char * cstring = call(s, to_cstring);
	for(int i = 0; i < s->length; ++i) {
		call(this, write, cstring[i]);
	}
}

OutputStream * OutputStream_new(OutputStream * this) {
    super(Object, OutputStream);
    super_method(Object, to_string);
    this->write = write;
    this->write_string = write_string;
    return this;
}

void OutputStream_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

