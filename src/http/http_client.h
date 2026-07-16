#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "../core/core.h"
#include "../log/log.h"
#include "http_types.h"


typedef struct {
    inherits(Object);
    Logger * _logger;

    HttpResponse * (*send_request)(ObjectPtr, HttpRequest *);
} HttpClient;

HttpClient * HttpClient_new(HttpClient *);
void HttpClient_delete(ObjectPtr);

#endif
