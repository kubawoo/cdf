#include "loggerfactory.h"


static Logger * _get_logger(ObjectPtr _this, String * name) {
	make_this(LoggerFactory, _this);
	Logger * log = call(this->_loggers, get, name);
	if(log == NULL) {
        log = new(Logger, name, LOG_LEVEL_OFF);
		call(this->_loggers, put, name, log); 
	}
	return log;
}

static Logger * _get_logger_cstring(ObjectPtr _this, const char * name) {
    String * s = new(String, name);
    Logger * logger = _get_logger(_this, s);
    REFCDEC(s);
    return logger;
}

LoggerFactory * LoggerFactory_new(LoggerFactory * this) {
    super(Object, LoggerFactory);
    this->_loggers = new(Map);
    this->get_logger = _get_logger;
    this->get_logger_cstring = _get_logger_cstring;
    return this;
}

void LoggerFactory_delete(ObjectPtr _this) {
    make_this(LoggerFactory, _this);
    REFCDEC(this->_loggers);
    super_delete(Object, this);
}

