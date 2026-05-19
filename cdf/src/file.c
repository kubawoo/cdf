#include "file.h"
#include <unistd.h>

static bool open(ObjectPtr _this, String * mode) {
    make_this(File, _this);
    if (this->_fp) {
        fclose(this->_fp);
    }
    this->_fp = fopen(call(this->_path, to_cstring), call(mode, to_cstring));
    return this->_fp != NULL;
}

static void _close(ObjectPtr _this) {
    make_this(File, _this);
    if (this->_fp) {
        fclose(this->_fp);
        this->_fp = NULL;
    }
}

static bool is_open(ObjectPtr _this) {
    make_this(File, _this);
    return this->_fp != NULL;
}

static bool exists(ObjectPtr _this) {
    make_this(File, _this);
    return access(call(this->_path, to_cstring), F_OK) == 0;
}

static int read_char(ObjectPtr _this) {
    make_this(File, _this);
    if (!this->_fp) {
        return -1;
    }
    return fgetc(this->_fp);
}

static String * _read(ObjectPtr _this) {
    make_this(File, _this);
    if (!this->_fp) {
        return NULL;
    }
    String * result = new(String);
    int c;
    while ((c = fgetc(this->_fp)) != EOF) {
        call(result, append_char, (char)c);
    }
    return result;
}

static bool write_char(ObjectPtr _this, char c) {
    make_this(File, _this);
    if (!this->_fp) {
        return false;
    }
    return fputc(c, this->_fp) != EOF;
}

static bool write_cstring(ObjectPtr _this, const char * str) {
    make_this(File, _this);
    if (!this->_fp) {
        return false;
    }
    return fputs(str, this->_fp) != EOF;
}

static bool write_string(ObjectPtr _this, String * s) {
    make_this(File, _this);
    return call(this, write_cstring, call(s, to_cstring));
}

static bool flush(ObjectPtr _this) {
    make_this(File, _this);
    if (!this->_fp) {
        return false;
    }
    return fflush(this->_fp) == 0;
}

File * File_new1(File * this, String * path) {
    super(Object, File);
    this->open = open;
    this->close = _close;
    this->is_open = is_open;
    this->exists = exists;
    this->read_char = read_char;
    this->read = _read;
    this->write_char = write_char;
    this->write_cstring = write_cstring;
    this->write_string = write_string;
    this->flush = flush;
    this->_fp = NULL;
    this->_path = path;
    REFCINC(path);
    return this;
}

void File_delete(ObjectPtr _this) {
    make_this(File, _this);
    _close(_this);
    REFCDEC(this->_path);
    super_delete(Object, _this);
}
