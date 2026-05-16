#include "test_framework.h"
#include "http_client.h"
#include "http_server.h"
#include <unistd.h>


void get_html_test(TEST_CASE_ARGUMENTS) {
    HttpServer * server = new(HttpServer, 19876, NULL);

    call(server, start);
    ASSERT_TRUE(server->run);
    String * loc = new(String, "http://localhost:19876/index.html");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    HttpClient * client = new(HttpClient);
    HttpResponse * response = call(client, send_request, request);
    ASSERT_EQUAL(HTTP_STATUS_OK, response->status);

    fprintf(stderr, "I'm still running\n");
    sleep(1);
    call(server, stop);
    REFCDEC(server);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(response);
    REFCDEC(client);
}


TEST_CASES_BEGIN
    TEST_CASE(get_html_test);
TEST_CASES_END


