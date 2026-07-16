#ifndef CDF_HTTP_UTILS_H
#define CDF_HTTP_UTILS_H

#include <stdbool.h>

bool _http_send_all(int sock, const void * data, int length);

#endif