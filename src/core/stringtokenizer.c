#include "stringtokenizer.h"


void StringTokenizer_delete(ObjectPtr _this) {
    make_this(StringTokenizer, _this);
    REFCDEC(this->_content);
    super_delete(Object, _this);
}

static List* split_by_char(ObjectPtr _this, const char c) {
    make_this(StringTokenizer, _this);
    List * l = new(List);
    int pos_start = 0;
    while(true) {
        int pos_end = call(this->_content, next_index_of_char, pos_start, c);
        if(pos_end < 0) {
            if(pos_start < this->_content->length) {
                String *s = call(this->_content, substring_from, pos_start);
                call(l, add, s);
                REFCDEC(s);
            }
            break;
        }
        if(pos_end == pos_start) {
            ++pos_start;
            continue;
        }

        String *s = call(this->_content, substring, pos_start, pos_end);
        call(l, add, s);
        REFCDEC(s);
        pos_start = pos_end + 1;
    }
    return l;
}

StringTokenizer * StringTokenizer_new1(StringTokenizer * this, String * s) {
    super(Object, StringTokenizer);
    this->split_by_char = split_by_char;
    this->_content = s;
    REFCINC(s);
    return this;
}

