#include "http_types.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>


static void to_string_test(void)
{
    String * loc = new(String, "http://www.example.com");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 0\r\n\r\n") == 0);
    REFCDEC(s);

    s = call(request->schema, to_string);
    assert(strcmp(call(s, to_cstring), "http") == 0);
    REFCDEC(s);

    s = call(request->host, to_string);
    assert(strcmp(call(s, to_cstring), "www.example.com") == 0);
    REFCDEC(s);

    s = call(request->path, to_string);
    assert(strcmp(call(s, to_cstring), "/") == 0);
    REFCDEC(s);

    assert(request->port != NULL);
    assert((request->port->value) == (80));

    REFCDEC(loc);
    REFCDEC(request);
}

static void schena_host_port_path_test1(void)
{
    String * loc = new(String, "http://www.example.com:88");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com:88\r\nContent-Length: 0\r\n\r\n") == 0);
    REFCDEC(s);

    s = call(request->schema, to_string);
    assert(strcmp(call(s, to_cstring), "http") == 0);
    REFCDEC(s);

    s = call(request->host, to_string);
    assert(strcmp(call(s, to_cstring), "www.example.com") == 0);
    REFCDEC(s);

    s = call(request->path, to_string);
    assert(strcmp(call(s, to_cstring), "/") == 0);
    REFCDEC(s);

    assert(request->port != NULL);
    assert((request->port->value) == (88));

    REFCDEC(loc);
    REFCDEC(request);
}


static void schena_host_port_path_test2(void)
{
    String * loc = new(String, "http://www.example.com:88/");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com:88\r\nContent-Length: 0\r\n\r\n") == 0);
    REFCDEC(s);

    s = call(request->schema, to_string);
    assert(strcmp(call(s, to_cstring), "http") == 0);
    REFCDEC(s);

    s = call(request->host, to_string);
    assert(strcmp(call(s, to_cstring), "www.example.com") == 0);
    REFCDEC(s);

    s = call(request->path, to_string);
    assert(strcmp(call(s, to_cstring), "/") == 0);
    REFCDEC(s);

    assert(request->port != NULL);
    assert((request->port->value) == (88));

    REFCDEC(loc);
    REFCDEC(request);
}


static void schena_host_port_path_test3(void)
{
    String * loc = new(String, "https://www.example.com/");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 0\r\n\r\n") == 0);
    REFCDEC(s);

    s = call(request->schema, to_string);
    assert(strcmp(call(s, to_cstring), "https") == 0);
    REFCDEC(s);

    s = call(request->host, to_string);
    assert(strcmp(call(s, to_cstring), "www.example.com") == 0);
    REFCDEC(s);

    s = call(request->path, to_string);
    assert(strcmp(call(s, to_cstring), "/") == 0);
    REFCDEC(s);

    assert(request->port != NULL);
    assert((request->port->value) == (80));

    REFCDEC(loc);
    REFCDEC(request);
}


static void schena_host_port_path_test4(void)
{
    String * loc = new(String, "www.example.com:88/foo/bar/baz.html");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET /foo/bar/baz.html HTTP/1.1\r\nHost: www.example.com:88\r\nContent-Length: 0\r\n\r\n") == 0);
    REFCDEC(s);

    s = call(request->schema, to_string);
    assert(strcmp(call(s, to_cstring), "http") == 0);
    REFCDEC(s);

    s = call(request->host, to_string);
    assert(strcmp(call(s, to_cstring), "www.example.com") == 0);
    REFCDEC(s);

    s = call(request->path, to_string);
    assert(strcmp(call(s, to_cstring), "/foo/bar/baz.html") == 0);
    REFCDEC(s);

    assert(request->port != NULL);
    assert((request->port->value) == (88));

    REFCDEC(loc);
    REFCDEC(request);
}

static void schena_host_port_path_test5(void)
{
    String * loc = new(String, "123.123.123.123:88/foo/bar/baz.html");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET /foo/bar/baz.html HTTP/1.1\r\nHost: 123.123.123.123:88\r\nContent-Length: 0\r\n\r\n") == 0);
    REFCDEC(s);

    s = call(request->schema, to_string);
    assert(strcmp(call(s, to_cstring), "http") == 0);
    REFCDEC(s);

    s = call(request->host, to_string);
    assert(strcmp(call(s, to_cstring), "123.123.123.123") == 0);
    REFCDEC(s);

    s = call(request->path, to_string);
    assert(strcmp(call(s, to_cstring), "/foo/bar/baz.html") == 0);
    REFCDEC(s);

    assert(request->port != NULL);
    assert((request->port->value) == (88));

    REFCDEC(loc);
    REFCDEC(request);
}

static void content_test(void)
{
    String * loc = new(String, "http://www.example.com");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "10")))));
    String * content = new(String, "1234567890");
    call(request, append_content, content);
    REFCDEC(content);

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 10\r\n\r\n1234567890") == 0);
    REFCDEC(s);

    REFCDEC(loc);
    REFCDEC(request);
}

static void query_parameters_test(void)
{
    String * loc = new(String, "http://www.example.com?foo=bar&baz=&foo2=123&bar2");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    String * s = call(request, to_string);
    assert(strcmp(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com?foo=bar&baz=&foo2=123&bar2\r\n\r\n") == 0);
    REFCDEC(s);

    assert(request->query_string != NULL);
    assert(strcmp(call(request->query_string, to_cstring), "foo=bar&baz=&foo2=123&bar2") == 0);
    assert(request->query_parameters != NULL);

    assert((call(request->query_parameters, get_length)) == (4));

    String * foo_key = new(String, "foo");
    String * foo_value = call(request->query_parameters, get, foo_key);
    assert(strcmp(call(foo_value, to_cstring), "bar") == 0);

    REFCDEC(foo_key);
    REFCDEC(foo_value);
    REFCDEC(loc);
    REFCDEC(request);

}
int main(void)
{
    to_string_test();
    schena_host_port_path_test1();
    schena_host_port_path_test2();
    schena_host_port_path_test3();
    schena_host_port_path_test4();
    schena_host_port_path_test5();
    content_test();
    query_parameters_test();
    return 0;
}



