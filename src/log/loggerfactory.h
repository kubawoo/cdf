#ifndef LOGGERFACTORY_H
#define LOGGERFACTORY_H

#include "logger.h"

typedef struct {
    inherits(Object);
	Map * _loggers;
    Logger * (*get_logger)(ObjectPtr, String * name);
    Logger * (*get_logger_cstring)(ObjectPtr, const char * name);
} LoggerFactory;

LoggerFactory * LoggerFactory_new(LoggerFactory *);
void LoggerFactory_delete(ObjectPtr);


#endif

