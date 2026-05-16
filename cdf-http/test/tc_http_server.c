#include "test_framework.h"
#include "http_server.h"

bool _parse_status_line(String * status_line, HttpRequest * request);

void status_line_test(TEST_CASE_ARGUMENTS) {
    HttpRequest * request = new(HttpRequest);
    String * status_line = new(String, "GET /index.html HTTP/1.1\r\nHost: localhost:1234\r\n\r\n");
    bool result = _parse_status_line(status_line, request);

    ASSERT_TRUE(result);
    ASSERT_EQUAL(request->method, HTTP_METHOD_GET);
    ASSERT_STRINGS_EQUAL(call(request->path, to_cstring), "/index.html");

    delete(status_line);
    delete(request);
}


TEST_CASES_BEGIN
    TEST_CASE(status_line_test);
TEST_CASES_END


