#include "http_types.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void to_string_test(void)
{
    HttpResponse * response = new(HttpResponse);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Date")), REFCTMP(new(String, "Thu, 22 Jan 2015 17:34:44 GMT")))));

    String * s = call(response, to_string);
    assert(strcmp(call(s, to_cstring), "HTTP/1.1 200 OK\r\n"
                         "Date: Thu, 22 Jan 2015 17:34:44 GMT\r\n"
                         "\r\n") == 0);
    REFCDEC(s);
    REFCDEC(response);
}

static void to_string_with_content_test(void)
{
    HttpResponse * response = new(HttpResponse);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Date")), REFCTMP(new(String, "Thu, 22 Jan 2015 17:34:44 GMT")))));
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Type")), REFCTMP(new(String, "text/plain")))));
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "32")))));
    String * content = new(String, "this is the best content ever!\r\n");
    call(response, append_content, content);
    REFCDEC(content);

    String * s = call(response, to_string);
    assert(strcmp(call(s, to_cstring), "HTTP/1.1 200 OK\r\n"
                         "Date: Thu, 22 Jan 2015 17:34:44 GMT\r\n"
                         "Content-Type: text/plain\r\n"
                         "Content-Length: 32\r\n"
                         "\r\n"
                         "this is the best content ever!\r\n") == 0);
    REFCDEC(s);
    REFCDEC(response);
}
int main(void)
{
    to_string_test();
    to_string_with_content_test();
    return 0;
}



