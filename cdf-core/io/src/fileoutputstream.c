#include "fileoutputstream.h"

static void write(ObjectPtr _this, char c) {
    make_this(FileOutputStream, _this);
    if(this->_file) {
        fputc(c, this->_file);
    }
}


FileOutputStream * FileOutputStream_new2(FileOutputStream * this, String * filename, bool append) {
    super(OutputStream, FileOutputStream);
    override(OutputStream, write, write);
    if(append) {
        this->_file = fopen(call(filename, to_cstring), "a");
    } else {
        this->_file = fopen(call(filename, to_cstring), "w");
    }
    return this;
}


FileOutputStream * FileOutputStream_new1(FileOutputStream* this, String* filename) {
    this(FileOutputStream, filename, false);
    return this;
}


void FileOutputStream_delete(ObjectPtr _this) {
    make_this(FileOutputStream, _this);
    fclose(this->_file);
    super_delete(OutputStream, _this);
}

