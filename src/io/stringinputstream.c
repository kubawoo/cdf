#include "stringinputstream.h"

static int read(ObjectPtr _this) {
    make_this(StringInputStream, _this);
    if(this->_pos < this->_txt->length) {
        /* Widened to an unsigned value: a plain char promotes with its sign bit
           set for every byte above 0x7F, which would look like end-of-input and
           truncate the text partway through a UTF-8 sequence. */
        return (unsigned char) call(this->_txt, to_cstring)[this->_pos++];
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

