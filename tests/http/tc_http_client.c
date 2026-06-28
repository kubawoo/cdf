#include "http_client.h"
#include "http_server.h"
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>


static void get_html_test(void)
{
    HttpServer * server = new(HttpServer, 19876, NULL);

    call(server, start);
    assert(server->run);
    String * loc = new(String, "http://localhost:19876/index.html");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    HttpClient * client = new(HttpClient);
    HttpResponse * response = call(client, send_request, request);
    assert((HTTP_STATUS_OK) == (response->status));

    fprintf(stderr, "I'm still running\n");
    sleep(1);
    call(server, stop);
    REFCDEC(server);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(response);
    REFCDEC(client);
}
int main(void)
{
    get_html_test();
    return 0;
}



