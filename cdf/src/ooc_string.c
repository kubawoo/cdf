#include "ooc_string.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

void String_delete(void * _this) {
    String * this = (String *) _this;
    if(this->_content != NULL) {
        free(this->_content);
    }
    super_delete(Object, _this);
}

void _String_resize(void * _this, size_t len) {
    len++; // one more for '\0'
    String * this = (String *) _this;
    if(this->_allocated <= len) {
        this->_content = realloc(this->_content, len * sizeof(char));
        this->_allocated = len;
    }
}

void String_set_text(void * _this, const char * text) {
    String * this = (String *) _this;
    size_t len = strlen(text);
    _String_resize(this, len);
    strcpy(this->_content, text);
    this->length = len;
}

const char * String_to_cstring(void * _this) {
    String * this = (String *) _this;
    return this->_content;
}

String * String_to_string(void * _this) {
    REFCINC(_this);
    return _this;
}

void String_append_cstring(void * _this, const char * txt) {
    String * this = (String *) _this;
    size_t len = strlen(txt) + this->length;
    _String_resize(this, len);
    strcat(this->_content, txt);
    this->length = len;
}

void String_append(void * _this, String * str) {
    String * this = (String *) _this;
    size_t len = this->length + str->length;
    _String_resize(this, len);
    strcat(this->_content, str->_content);
    this->length = len;
}

void String_append_char(void * _this, const char c) {
    make_this(String, _this);
    size_t len = this->length + 1;
    _String_resize(this, len);
    this->_content[len - 1] = c;
    this->_content[len - 0] = '\0';
    this->length = len;
}

void String_clear(void * _this) {
    make_this(String, _this);
    this->length = 0;
    this->_content[0] = '\0';
}

void trim_left(String * s) {
    int offset = 0;
    while(isspace(s->_content[offset]) && offset < s->length) {
        offset++;
    }

    if(offset == s->length) {
        s->_content[0] = '\0';
        s->length = 0;
    } else {
        for(int i = 0; i + offset < s->length; ++i) {
            s->_content[i] = s->_content[i + offset];
            s->_content[i + offset] = '\0';
        }
        s->length = s->length - offset;
    }
}

void trim_right(String * s) {
    for(int i = s->length - 1; i >= 0; --i) {
        if(!isspace(s->_content[i])) {
            s->_content[i+1] = '\0';
            s->length = i+1;
            break;
        }
    }
}

void String_trim(void * _this) {
    make_this(String, _this);

    if(this->length > 0 && isspace(this->_content[0])) {
        trim_left(this);
    }

    if(this->length > 0 && isspace(this->_content[this->length - 1])) {
        trim_right(this);
    }
}

bool String_equals_cstring(void * _this, const char * string) {
    if(string == NULL) {
        return false;
    }
    make_this(String, _this);
    return strcmp(this->_content, string) == 0;
}

bool String_equals(void * _this, void * _other) {
    if(_this == _other) {
        return true;
    }

    if(_other == NULL) {
        return false;
    }

    make_this(String, _this);

    if(!type_equal(_other, "String")) {
        return false;
    }

    String * other = (String *) _other;
    return this->length == other->length && strcmp(this->_content, other->_content) == 0;
}


bool String_contains_any_char(void * _this, const char * characters) {
    make_this(String, _this);
    const char * name_cstring = call(this, to_cstring);
    for(int i = 0; i < strlen(characters); ++i) {
        char c = characters[i];
        if(strchr(name_cstring, c) != NULL) {
            return true;
       }
    }
    return false;
}

bool String_contains_any(void * _this, String * characters) {
    return String_contains_any_char(_this, call(characters, to_cstring));
}

ObjectPtr String_copy(void * _this) {
    make_this(String, _this);
    return new(String, this->_content);
}

int String_next_index_of_char(void * _this, int start, const char c) {
    make_this(String, _this);
    for(int i = start; i < this->length; ++i) {
        if(this->_content[i] == c) {
            return i;
        }
    }
    return -1;
}

int String_index_of_char(void *_this, const char c) {
    return String_next_index_of_char(_this, 0, c);
}

int String_next_index_of_cstring(void * _this, int start, const char * cstring) {
    make_this(String, _this);
    int cstring_len = strlen(cstring);
    for(int i = start; i < this->length ; ++i) {
        bool equal = false;
        for(int j = 0; j < cstring_len; ++j) {
            char c1 = this->_content[i+j];
            char c2 = cstring[j];
            if(c2 != c1) {
                equal = false;
                break;
            } else {
                equal = true;
            }
        }
        if(equal) {
            return i;
        }
    }
    return -1;
}

int String_index_of_cstring(void * _this, const char * cstring) {
    return String_next_index_of_cstring(_this, 0, cstring);
}

int String_next_index_of_string(void * _this, int start, String * string) {
    return String_next_index_of_cstring(_this, start, string->_content);
}

int String_index_of_string(void * _this, String * string) {
    return String_index_of_cstring(_this, string->_content);
}


String * String_substring(void * _this, int from, int to) {
    make_this(String, _this);
    String * sub = new(String);
    int len = to - from;
    _String_resize(sub, len);
    sub->length = len;
    for(int i = 0; i < len; ++i) {
        sub->_content[i] = this->_content[from + i];
    }
    sub->_content[len] = '\0';
    return sub;
}

String * String_substring_from(void * _this, int from) {
    make_this(String, _this);
    return String_substring(this, from, this->length);
}

void format(ObjectPtr _this, const char * fmt, ...) {
    make_this(String, _this);
    call(this, clear);
    va_list argptr;
    va_start(argptr, fmt);

    int required_size = vsnprintf(this->_content, this->_allocated, fmt, argptr);

    if(required_size > this->length) {
        va_list argptr2;
        va_start(argptr2, fmt);
        _String_resize(this, required_size);
        vsnprintf(this->_content, this->_allocated, fmt, argptr2);
        this->length = required_size;
        va_end(argptr2);
    } else {
        this->length = strlen(this->_content);
    }

    va_end(argptr);
}

const char String_char_at(ObjectPtr _this, int pos) {
    make_this(String, _this);
    if(pos >= 0 && pos < this->length) {
        return this->_content[pos];
    }
    return 0;
}

String * String_new(String * this) {
    super(Object, String);
    override(Object, to_string, String_to_string);
    override(Object, equals, String_equals);
    override(Object, copy, String_copy);
    this->length = 0;
    this->_allocated = 0;
    this->_content = NULL;
    _String_resize(this, this->length);
    this->_content[0] = '\0';
    this->set_text = String_set_text;
    this->to_cstring = String_to_cstring;
    this->append = String_append;
    this->append_cstring = String_append_cstring;
    this->append_char = String_append_char;
    this->clear = String_clear;
    this->trim = String_trim;
    this->contains_any = String_contains_any;
    this->contains_any_char = String_contains_any_char;
    this->equals_cstring = String_equals_cstring;
    this->index_of_char = String_index_of_char;
    this->next_index_of_char = String_next_index_of_char;
    this->index_of_cstring = String_index_of_cstring;
    this->next_index_of_cstring = String_next_index_of_cstring;
    this->index_of_string = String_index_of_string;
    this->next_index_of_string = String_next_index_of_string;
    this->substring = String_substring;
    this->substring_from = String_substring_from;
    this->format = format;
    this->char_at = String_char_at;

    return this;
}

String * String_new1(String * this, const char * txt) {
    this(String);
    call(this, set_text, txt);
    return this;
}
