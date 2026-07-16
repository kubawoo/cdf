#include "logger.h"
#include <stdio.h>


static const char * _loglevel_to_cstring(LogLevel level) {
	switch(level) {
		case LOG_LEVEL_OFF:
		case LOG_LEVEL_ERROR:
			return "ERROR";
		case LOG_LEVEL_WARN:
			return "WARN";
		case LOG_LEVEL_INFO:
			return "INFO";
		case LOG_LEVEL_DEBUG:
			return "DEBUG";
		case LOG_LEVEL_TRACE:
			return "TRACE";
	}
	return "";
}

static bool _is_enabled(ObjectPtr _this, LogLevel level) {
    make_this(Logger, _this);
    return level <= this->current_level;
}

static void _log(ObjectPtr _this, LogLevel level, String * msg, const char * filename, int line) {
    if(_is_enabled(_this, level)) {
		DateTime * now = new(DateTime);
		String * now_string = call(now, format, "%F %T");
		REFCDEC(now);
		String * s = new(String);
		call(s, format, "%s %-5s [%s:%d] %s",
					call(now_string, to_cstring), _loglevel_to_cstring(level), filename, line, call(msg, to_cstring));
		REFCDEC(now_string);
		printf("%s\n", call(s, to_cstring));
		fflush(stdout);
		REFCDEC(s);
	}
}


Logger * Logger_new2(Logger * this, String * name, LogLevel level) {
    super(Object, Logger);
    REFCINC(name);
    this->name = name;
    this->current_level = level;

    this->log = _log;
    this->is_enabled = _is_enabled;
    return this;
}

void Logger_delete(ObjectPtr _this) {
    make_this(Logger, _this);
    REFCDEC(this->name);
    super_delete(Object, this);
}

