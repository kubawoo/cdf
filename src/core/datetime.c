#include "datetime.h"
#include <stdio.h>

static String * DateTime_format(ObjectPtr _this, const char * format) {
    make_this(DateTime, _this);
    char stack_buf[256];
    size_t len = strftime(stack_buf, sizeof(stack_buf), format, &this->_time);
    if (len > 0 && len < sizeof(stack_buf)) {
        return new(String, stack_buf);
    }
    size_t buffer_length = 512;
    while (1) {
        char * buffer = malloc(buffer_length);
        if (!buffer) return NULL;
        len = strftime(buffer, buffer_length, format, &this->_time);
        if (len > 0 && len < buffer_length) {
            String * s = new(String, buffer);
            free(buffer);
            return s;
        }
        free(buffer);
        buffer_length *= 2;
    }
}

static String * DateTime_to_string(ObjectPtr _this) {
    return DateTime_format(_this, "%FT%T");
}

static DateTime * _base_new(DateTime * this) {
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
