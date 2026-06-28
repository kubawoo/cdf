#include "fileoutputstream.h"

static void write(ObjectPtr _this, char c) {
    make_this(FileOutputStream, _this);
    if (this->_file) {
        call(this->_file, write_char, c);
    }
}


FileOutputStream * FileOutputStream_new1(FileOutputStream * this, File * file) {
    this(FileOutputStream, file, false);
    return this;
}

FileOutputStream * FileOutputStream_new2(FileOutputStream * this, File * file, bool append) {
    super(OutputStream, FileOutputStream);
    override(OutputStream, write, write);
    this->_file = file;
    REFCINC(file);
    call(this->_file, open, append ? "a" : "w");
    return this;
}

void FileOutputStream_delete(ObjectPtr _this) {
    make_this(FileOutputStream, _this);
    call(this->_file, close);
    REFCDEC(this->_file);
    super_delete(OutputStream, _this);
}

