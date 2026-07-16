#include <core.h>
#include <http.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#define PORT 19876
HttpServer * server = NULL;


typedef struct {
    inherits(HttpRequestHandler);
    void (*handle)(void *, HttpRequest *, HttpResponse *);
} WwwRequestHandler;

void WwwRequestHandler_handle(void * _this, HttpRequest * request, HttpResponse * response) {
    String * content = new(String, "<html><body bgcolor=\"#abcdef\"><h3>Hello CDF world!</h3></body></html>");
    call(response, append_content, content);
    REFCDEC(content);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Type")), REFCTMP(new(String, "text/html")))));
}

void WwwRequestHandler_delete(void * _this) {
    super_delete(HttpRequestHandler, _this);
}

WwwRequestHandler * WwwRequestHandler_new(WwwRequestHandler * this) {
    super(HttpRequestHandler, WwwRequestHandler);
    override(HttpRequestHandler, handle, WwwRequestHandler_handle);
    return this;
}


void signal_handler(int signal) {
    switch(signal) {
    case SIGINT:
        printf("\nStopping the server...\n");
        if(server != NULL) {
            call(server, stop);
        }
        break;
    }
}

int main(void) {
    signal(SIGINT, signal_handler);
    server = new(HttpServer, PORT, (HttpRequestHandler *) new(WwwRequestHandler));
    printf("Starting the server on port %d\n", PORT);
    call(server, start);
    if(call(server, is_running)) {
        printf("Server started\n");
    }
    while(call(server, is_running)) {
        sleep(1);
    }
    REFCDEC(server);
    printf("Bye bye\n");
    return 0;
}


