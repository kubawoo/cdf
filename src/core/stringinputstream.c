#include "stringinputstream.h"

static int read(ObjectPtr _this) {
    make_this(StringInputStream, _this);
    if(this->_pos < this->_txt->length) {
        return call(this->_txt, to_cstring)[this->_pos++];
    }
    return -1;
}


StringInputStream * StringInputStream_new1(StringInputStream * this, String *txt) {
    super(InputStream, StringInputStream);
    override(InputStream, read, read);
    this->_pos = 0;
    this->_txt = txt;
    REFCINC(txt);
    return this;
}

void StringInputStream_delete(ObjectPtr _this) {
    make_this(StringInputStream, _this);
    REFCDEC(this->_txt);
    super_delete(InputStream, _this);
}

