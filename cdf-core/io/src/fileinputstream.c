#include "fileinputstream.h"

static int read(ObjectPtr _this) {
    make_this(FileInputStream, _this);
    if(this->_file == NULL) {
        return -1;
    }
    return fgetc(this->_file);
}

FileInputStream * FileInputStream_new1(FileInputStream * this, String *filename) {
    super(InputStream, FileInputStream);
    override(InputStream, read, read);
    this->_file = fopen(call(filename, to_cstring), "r");
    return this;
}

void FileInputStream_delete(ObjectPtr _this) {
    make_this(FileInputStream, _this);
    if(this->_file != NULL) {
        fclose(this->_file);
    }
    super_delete(InputStream, _this);
}

