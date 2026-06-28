#include "http_server.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

bool _parse_status_line(String * status_line, HttpRequest * request);

static void status_line_test(void)
{
    HttpRequest * request = new(HttpRequest);
    String * status_line = new(String, "GET /index.html HTTP/1.1\r\nHost: localhost:1234\r\n\r\n");
    bool result = _parse_status_line(status_line, request);

    assert(result);
    assert((request->method) == (HTTP_METHOD_GET));
    assert(strcmp(call(request->path, to_cstring), "/index.html") == 0);

    delete(status_line);
    delete(request);
}
int main(void)
{
    status_line_test();
    return 0;
}



