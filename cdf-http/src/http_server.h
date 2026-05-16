#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <ooc.h>
#include <threads.h>
#include "http_types.h"

typedef struct {
    inherits(Object);
    void (*handle)(ObjectPtr, HttpRequest *, HttpResponse *);
} HttpRequestHandler;

HttpRequestHandler * HttpRequestHandler_new(HttpRequestHandler * this);
void HttpRequestHandler_delete(ObjectPtr);

typedef struct {
    inherits(Object);

    void (*start)(ObjectPtr);
    void (*stop)(ObjectPtr);

    HttpRequestHandler * _handler;

    thrd_t server_thread;
    mtx_t run_mutex;
    bool run;
    int server_socket;
    int port;
    bool (*is_running)(ObjectPtr);
} HttpServer;

HttpServer * HttpServer_new2(HttpServer *, int, HttpRequestHandler *);
void HttpServer_delete(ObjectPtr);

typedef struct {
    inherits(Object);
    int conn_fd;
    HttpRequestHandler * handler;
} _ConnectionData;
_ConnectionData * _ConnectionData_new2(_ConnectionData *, int, HttpRequestHandler *);
void _ConnectionData_delete(ObjectPtr);


#endif
