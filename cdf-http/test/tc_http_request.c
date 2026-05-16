#include "test_framework.h"
#include "http_types.h"


void to_string_test(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "http://www.example.com");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 0\r\n\r\n");
    REFCDEC(s);

    s = call(request->schema, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "http");
    REFCDEC(s);

    s = call(request->host, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "www.example.com");
    REFCDEC(s);

    s = call(request->path, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "/");
    REFCDEC(s);

    ASSERT_NOT_NULL(request->port);
    ASSERT_EQUAL(request->port->value, 80);

    REFCDEC(loc);
    REFCDEC(request);
}

void schena_host_port_path_test1(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "http://www.example.com:88");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com:88\r\nContent-Length: 0\r\n\r\n");
    REFCDEC(s);

    s = call(request->schema, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "http");
    REFCDEC(s);

    s = call(request->host, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "www.example.com");
    REFCDEC(s);

    s = call(request->path, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "/");
    REFCDEC(s);

    ASSERT_NOT_NULL(request->port);
    ASSERT_EQUAL(request->port->value, 88);

    REFCDEC(loc);
    REFCDEC(request);
}


void schena_host_port_path_test2(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "http://www.example.com:88/");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com:88\r\nContent-Length: 0\r\n\r\n");
    REFCDEC(s);

    s = call(request->schema, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "http");
    REFCDEC(s);

    s = call(request->host, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "www.example.com");
    REFCDEC(s);

    s = call(request->path, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "/");
    REFCDEC(s);

    ASSERT_NOT_NULL(request->port);
    ASSERT_EQUAL(request->port->value, 88);

    REFCDEC(loc);
    REFCDEC(request);
}


void schena_host_port_path_test3(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "https://www.example.com/");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 0\r\n\r\n");
    REFCDEC(s);

    s = call(request->schema, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "https");
    REFCDEC(s);

    s = call(request->host, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "www.example.com");
    REFCDEC(s);

    s = call(request->path, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "/");
    REFCDEC(s);

    ASSERT_NOT_NULL(request->port);
    ASSERT_EQUAL(request->port->value, 80);

    REFCDEC(loc);
    REFCDEC(request);
}


void schena_host_port_path_test4(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "www.example.com:88/foo/bar/baz.html");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET /foo/bar/baz.html HTTP/1.1\r\nHost: www.example.com:88\r\nContent-Length: 0\r\n\r\n");
    REFCDEC(s);

    s = call(request->schema, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "http");
    REFCDEC(s);

    s = call(request->host, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "www.example.com");
    REFCDEC(s);

    s = call(request->path, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "/foo/bar/baz.html");
    REFCDEC(s);

    ASSERT_NOT_NULL(request->port);
    ASSERT_EQUAL(request->port->value, 88);

    REFCDEC(loc);
    REFCDEC(request);
}

void schena_host_port_path_test5(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "123.123.123.123:88/foo/bar/baz.html");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "0")))));

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET /foo/bar/baz.html HTTP/1.1\r\nHost: 123.123.123.123:88\r\nContent-Length: 0\r\n\r\n");
    REFCDEC(s);

    s = call(request->schema, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "http");
    REFCDEC(s);

    s = call(request->host, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "123.123.123.123");
    REFCDEC(s);

    s = call(request->path, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "/foo/bar/baz.html");
    REFCDEC(s);

    ASSERT_NOT_NULL(request->port);
    ASSERT_EQUAL(request->port->value, 88);

    REFCDEC(loc);
    REFCDEC(request);
}

void content_test(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "http://www.example.com");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    call(request, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "10")))));
    String * content = new(String, "1234567890");
    call(request, append_content, content);
    REFCDEC(content);

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 10\r\n\r\n1234567890");
    REFCDEC(s);

    REFCDEC(loc);
    REFCDEC(request);
}

void query_parameters_test(TEST_CASE_ARGUMENTS) {
    String * loc = new(String, "http://www.example.com?foo=bar&baz=&foo2=123&bar2");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    String * s = call(request, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring), "GET / HTTP/1.1\r\nHost: www.example.com?foo=bar&baz=&foo2=123&bar2\r\n\r\n");
    REFCDEC(s);

    ASSERT_NOT_NULL(request->query_string);
    ASSERT_STRINGS_EQUAL(call(request->query_string, to_cstring), "foo=bar&baz=&foo2=123&bar2");
    ASSERT_NOT_NULL(request->query_parameters);

    ASSERT_EQUAL(call(request->query_parameters, get_length), 4);

    String * foo_key = new(String, "foo");
    String * foo_value = call(request->query_parameters, get, foo_key);
    ASSERT_STRINGS_EQUAL(call(foo_value, to_cstring), "bar");

    REFCDEC(foo_key);
    REFCDEC(foo_value);
    REFCDEC(loc);
    REFCDEC(request);

}

TEST_CASES_BEGIN
    TEST_CASE(to_string_test);
    TEST_CASE(schena_host_port_path_test1);
    TEST_CASE(schena_host_port_path_test2);
    TEST_CASE(schena_host_port_path_test3);
    TEST_CASE(schena_host_port_path_test4);
    TEST_CASE(schena_host_port_path_test5);
    TEST_CASE(content_test);
    TEST_CASE(query_parameters_test);
TEST_CASES_END


