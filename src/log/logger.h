#ifndef LOGGER_H
#define LOGGER_H

#include "../core/core.h"

#define log_msg(x) x,__FILE_NAME__,__LINE__

typedef enum {
	LOG_LEVEL_OFF,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_TRACE
} LogLevel;

typedef struct {
    inherits(Object);
    String * name;
    LogLevel current_level;
    void (*log)(ObjectPtr _this, LogLevel level, String *msg , const char * filename, int line);
    bool (*is_enabled)(ObjectPtr _this, LogLevel level);
} Logger;

Logger * Logger_new2(Logger *, String * name, LogLevel level);
void Logger_delete(ObjectPtr);


#endif
