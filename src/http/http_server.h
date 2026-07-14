#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "../core/core.h"
#include <threads.h>
#include <time.h>
#include "http_types.h"

#define HTTP_SERVER_POOL_SIZE 8

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
    thrd_t _workers[HTTP_SERVER_POOL_SIZE];
    mtx_t run_mutex;
    bool run;
    int server_socket;
    int port;
    bool (*is_running)(ObjectPtr);

    mtx_t conn_mutex;
    cnd_t conn_cv;
    int _active_connections;
    String * _cached_date;
    time_t _last_date_update;
} HttpServer;

HttpServer * HttpServer_new2(HttpServer *, int, HttpRequestHandler *);
void HttpServer_delete(ObjectPtr);

typedef struct {
    inherits(Object);
    int conn_fd;
    HttpRequestHandler * handler;
    HttpServer * _server;
} _ConnectionData;
_ConnectionData * _ConnectionData_new3(_ConnectionData *, int, HttpRequestHandler *, HttpServer *);
void _ConnectionData_delete(ObjectPtr);


#endif
