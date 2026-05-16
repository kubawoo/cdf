#ifndef LOGGERFACTORY_H
#define LOGGERFACTORY_H

#include "logger.h"

typedef struct {
    inherits(Object);
	Map * _loggers;
    Logger * (*get_logger)(ObjectPtr, String * name);
} LoggerFactory;

LoggerFactory * LoggerFactory_new(LoggerFactory *);
void LoggerFactory_delete(ObjectPtr);


#endif

