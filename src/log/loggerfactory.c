#include "loggerfactory.h"


static Logger * _get_logger(ObjectPtr _this, String * name) {
	make_this(LoggerFactory, _this);
	Logger * log = call(this->_loggers, get, name);
	if(log == NULL) {
		log = new(Logger, name, LOG_LEVEL_INFO);
		call(this->_loggers, put, name, log); 
	}
	return log;
}

LoggerFactory * LoggerFactory_new(LoggerFactory * this) {
    super(Object, LoggerFactory);
    this->_loggers = new(Map);
    this->get_logger = _get_logger;
    return this;
}

void LoggerFactory_delete(ObjectPtr _this) {
    make_this(LoggerFactory, _this);
    REFCDEC(this->_loggers);
    super_delete(Object, this);
}

