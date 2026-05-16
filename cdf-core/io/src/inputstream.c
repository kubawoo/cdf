#include "inputstream.h"

static int read(ObjectPtr _this) {
    return -1;
}


InputStream * InputStream_new(InputStream * this) {
    super(Object, InputStream);
    this->read = read;
    return this;
}

void InputStream_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

