#ifndef CDF_CORE_DATETIME_H
#define CDF_CORE_DATETIME_H

#include <time.h>
#include "object.h"

typedef struct {
    inherits(Object);
    struct tm _time;

    String * (*to_string)(ObjectPtr);
    String * (*format)(ObjectPtr, const char *);

} DateTime;

DateTime * DateTime_new(DateTime * this);
DateTime * DateTime_new3(DateTime * this, int year, int month, int day);
DateTime * DateTime_new6(DateTime * this, int year, int month, int day, int hour, int minute, int second);
void DateTime_delete(ObjectPtr);

#endif
