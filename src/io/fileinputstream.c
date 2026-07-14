#include "fileinputstream.h"

static int read(ObjectPtr _this) {
    make_this(FileInputStream, _this);
    if (!this->_file) {
        return -1;
    }
    return call(this->_file, read_char);
}

FileInputStream * FileInputStream_new1(FileInputStream * this, File * file) {
    super(InputStream, FileInputStream);
    override(InputStream, read, read);
    this->_file = file;
    REFCINC(file);
    call(this->_file, open, "r");
    return this;
}

void FileInputStream_delete(ObjectPtr _this) {
    make_this(FileInputStream, _this);
    call(this->_file, close);
    REFCDEC(this->_file);
    super_delete(InputStream, _this);
}

