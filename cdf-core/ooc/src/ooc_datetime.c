#include "ooc_datetime.h"
#include <stdio.h>

String * DateTime_format(ObjectPtr _this, const char * format) {
    make_this(DateTime, _this);
    int buffer_length = 128;
    String * s = NULL;
    do {
        char * buffer = malloc(buffer_length * sizeof(char));
        size_t len = strftime(buffer, buffer_length, format, &this->_time);
        if(len < buffer_length) {
            s = new(String, buffer);
        }
        free(buffer);
        buffer_length *= 2;
    } while(s == NULL);
    return s;
}

String * DateTime_to_string(ObjectPtr _this) {
    return DateTime_format(_this, "%FT%T");
}

DateTime * _base_new(DateTime * this) {
    super(Object, DateTime);
    override(Object, to_string, DateTime_to_string);
    this->format = DateTime_format;
    memset(&this->_time, 0, sizeof(this->_time));
    return this;
}

DateTime * DateTime_new(DateTime * this) {
    this = _base_new(this);
    time_t now = time(NULL);
    struct tm * t = localtime(&now);
    this->_time = *t;
    return this;
}

DateTime * DateTime_new6(DateTime * this, int year, int month, int day, int hour, int minute, int second) {
    this = _base_new(this);
    this->_time.tm_year = year - 1900;
    this->_time.tm_mon = month - 1;
    this->_time.tm_mday = day;
    this->_time.tm_hour = hour;
    this->_time.tm_min = minute;
    this->_time.tm_sec = second;
    // adjusts the values
    mktime(&this->_time);
    return this;
}

DateTime * DateTime_new3(DateTime * this, int year, int month, int day) {
    return DateTime_new6(this, year, month, day, 0, 0, 0);
}

void DateTime_delete(ObjectPtr _this) {
    super_delete(Object,_this);
}
