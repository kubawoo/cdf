#include "test_framework.h"
#include "http_types.h"

void to_string_test(TEST_CASE_ARGUMENTS) {
    HttpResponse * response = new(HttpResponse);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Date")), REFCTMP(new(String, "Thu, 22 Jan 2015 17:34:44 GMT")))));

    String * s = call(response, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring),
                         "HTTP/1.1 200 OK\r\n"
                         "Date: Thu, 22 Jan 2015 17:34:44 GMT\r\n"
                         "\r\n");
    delete(s);
    delete(response);
}

void to_string_with_content_test(TEST_CASE_ARGUMENTS) {
    HttpResponse * response = new(HttpResponse);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Date")), REFCTMP(new(String, "Thu, 22 Jan 2015 17:34:44 GMT")))));
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Type")), REFCTMP(new(String, "text/plain")))));
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(new(String, "32")))));
    String * content = new(String, "this is the best content ever!\r\n");
    call(response, append_content, content);
    delete(content);

    String * s = call(response, to_string);
    ASSERT_STRINGS_EQUAL(call(s, to_cstring),
                         "HTTP/1.1 200 OK\r\n"
                         "Date: Thu, 22 Jan 2015 17:34:44 GMT\r\n"
                         "Content-Type: text/plain\r\n"
                         "Content-Length: 32\r\n"
                         "\r\n"
                         "this is the best content ever!\r\n");
    delete(s);
    delete(response);
}


TEST_CASES_BEGIN
    TEST_CASE(to_string_test);
    TEST_CASE(to_string_with_content_test);
TEST_CASES_END


