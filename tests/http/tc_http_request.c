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
    // The query string must not be absorbed into the host.
    assert(strcmp(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n") == 0);
    REFCDEC(s);

    assert(request->query_string != NULL);
    assert(strcmp(call(request->query_string, to_cstring), "foo=bar&baz=&foo2=123&bar2") == 0);
    assert(request->query_parameters != NULL);

    assert((call(request->query_parameters, size)) == (4));

    String * foo_key = new(String, "foo");
    String * foo_value = call(request->query_parameters, get, foo_key);
    assert(strcmp(call(foo_value, to_cstring), "bar") == 0);

    REFCDEC(foo_key);
    REFCDEC(foo_value);
    REFCDEC(loc);
    REFCDEC(request);

}
static void colon_in_path_test(void)
{
    // A colon after the path separator is part of the path, not a port.
    String * loc = new(String, "http://h/pa:th");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    assert(strcmp(call(request->host, to_cstring), "h") == 0);
    assert((request->port->value) == (80));
    assert(strcmp(call(request->path, to_cstring), "/pa:th") == 0);
    REFCDEC(loc);
    REFCDEC(request);
}

static void colon_in_path_with_port_test(void)
{
    // Both a real port and a colon in the path must be honoured.
    String * loc = new(String, "http://h:88/pa:th");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    assert(strcmp(call(request->host, to_cstring), "h") == 0);
    assert((request->port->value) == (88));
    assert(strcmp(call(request->path, to_cstring), "/pa:th") == 0);
    REFCDEC(loc);
    REFCDEC(request);
}

static void empty_port_test(void)
{
    // "host:" with no digits must not produce port 0.
    String * loc = new(String, "http://h:");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    assert(strcmp(call(request->host, to_cstring), "h") == 0);
    assert((request->port->value) == (80));
    assert(strcmp(call(request->path, to_cstring), "/") == 0);
    REFCDEC(loc);
    REFCDEC(request);
}

static void unparsed_request_to_string_test(void)
{
    // A request with no location has method UNKNOWN and a NULL path. Neither
    // may be dereferenced when serialising.
    HttpRequest * request = new(HttpRequest);
    assert(request->method == HTTP_METHOD_UNKNOWN);
    assert(request->path == NULL);

    String * s = call(request, to_string);
    assert(s != NULL);
    assert(strcmp(call(s, to_cstring), "UNKNOWN / HTTP/1.1\r\n\r\n") == 0);
    REFCDEC(s);
    REFCDEC(request);
}

static void unknown_method_with_location_test(void)
{
    // Location is parsed, so path is set, but the method is still unknown.
    String * loc = new(String, "http://www.example.com");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_UNKNOWN, loc);
    assert(request->path != NULL);

    String * s = call(request, to_string);
    assert(s != NULL);
    assert(strcmp(call(s, to_cstring),
                  "UNKNOWN / HTTP/1.1\r\nHost: www.example.com\r\n\r\n") == 0);
    REFCDEC(s);
    REFCDEC(loc);
    REFCDEC(request);
}

static void null_path_to_string_test(void)
{
    // A path can be cleared independently of the method.
    String * loc = new(String, "http://www.example.com");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    REFCDEC(request->path);
    request->path = NULL;

    String * s = call(request, to_string);
    assert(s != NULL);
    assert(strcmp(call(s, to_cstring),
                  "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n") == 0);
    REFCDEC(s);
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
    colon_in_path_test();
    colon_in_path_with_port_test();
    empty_port_test();
    unparsed_request_to_string_test();
    unknown_method_with_location_test();
    null_path_to_string_test();
    return 0;
}



