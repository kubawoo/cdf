#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <ooc.h>
#include "http_types.h"


typedef struct {
    inherits(Object);

    HttpResponse * (*send_request)(ObjectPtr, HttpRequest *);
} HttpClient;

HttpClient * HttpClient_new(HttpClient *);
void HttpClient_delete(ObjectPtr);

#endif
