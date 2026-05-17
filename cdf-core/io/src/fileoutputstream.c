#include "fileoutputstream.h"

static void write(ObjectPtr _this, char c) {
    make_this(FileOutputStream, _this);
    if (this->_file) {
        call(this->_file, write_char, c);
    }
}

FileOutputStream * FileOutputStream_new2(FileOutputStream * this, String * filename, bool append) {
    super(OutputStream, FileOutputStream);
    override(OutputStream, write, write);
    this->_file = new(File, filename);
    call(this->_file, open, new(String, append ? "a" : "w"));
    return this;
}

FileOutputStream * FileOutputStream_new1(FileOutputStream* this, String* filename) {
    this(FileOutputStream, filename, false);
    return this;
}

FileOutputStream * FileOutputStream_new3(FileOutputStream * this, File * file, bool append) {
    super(OutputStream, FileOutputStream);
    override(OutputStream, write, write);
    this->_file = file;
    REFCINC(file);
    return this;
}

void FileOutputStream_delete(ObjectPtr _this) {
    make_this(FileOutputStream, _this);
    REFCDEC(this->_file);
    super_delete(OutputStream, _this);
}

