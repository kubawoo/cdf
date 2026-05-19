#include "fileinputstream.h"

static int read(ObjectPtr _this) {
    make_this(FileInputStream, _this);
    if (!this->_file) {
        return -1;
    }
    return call(this->_file, read_char);
}

FileInputStream * FileInputStream_new1(FileInputStream * this, String * filename) {
    super(InputStream, FileInputStream);
    override(InputStream, read, read);
    this->_file = new(File, filename);
    call(this->_file, open, new(String, "r"));
    return this;
}

FileInputStream * FileInputStream_new2(FileInputStream * this, File * file) {
    super(InputStream, FileInputStream);
    override(InputStream, read, read);
    this->_file = file;
    REFCINC(file);
    return this;
}

void FileInputStream_delete(ObjectPtr _this) {
    make_this(FileInputStream, _this);
    REFCDEC(this->_file);
    super_delete(InputStream, _this);
}

