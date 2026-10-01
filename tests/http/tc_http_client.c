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
#include <threads.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


static void get_html_test(void)
{
    HttpServer * server = new(HttpServer, 19876, NULL);

    call(server, start);
    assert(server->run);
    String * loc = new(String, "http://localhost:19876/index.html");
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);
    HttpClient * client = new(HttpClient);
    HttpResponse * response = call(client, send_request, request);
    assert(response != NULL);
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

/*
 * Minimal raw-socket server used to drive the client into its failure paths.
 * The CDF server always emits well-formed responses, so a plain socket is the
 * only way to make the client see something it cannot parse.
 */
typedef struct {
    int listen_fd;
    int port;
    const char * response;
} _FakeServer;

static int _fake_server_main(void * arg) {
    _FakeServer * fs = (_FakeServer *) arg;
    // Serve exactly one connection, then exit, so thrd_join always returns.
    int c = accept(fs->listen_fd, NULL, NULL);
    if(c >= 0) {
        char buf[2048];
        recv(c, buf, sizeof(buf), 0);
        if(fs->response && fs->response[0] != '\0') {
            send(c, fs->response, strlen(fs->response), 0);
        }
        close(c);
    }
    return 0;
}

// Binds an ephemeral port so this test cannot collide with anything else.
static int _listen_ephemeral(int * port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 0;
    assert(bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == 0);
    socklen_t len = sizeof(addr);
    assert(getsockname(fd, (struct sockaddr *) &addr, &len) == 0);
    assert(listen(fd, 8) == 0);
    *port = ntohs(addr.sin_port);
    return fd;
}

// A response whose status line cannot be parsed must yield NULL, not a crash.
static void malformed_response_returns_null(void)
{
    _FakeServer fs;
    fs.response = "GARBAGE\r\n\r\n";
    fs.listen_fd = _listen_ephemeral(&fs.port);

    thrd_t th;
    assert(thrd_create(&th, _fake_server_main, &fs) == thrd_success);

    HttpClient * client = new(HttpClient);
    String * loc = new(String);
    call(loc, format, "http://127.0.0.1:%d/", fs.port);
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    HttpResponse * response = call(client, send_request, request);
    assert(response == NULL);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(client);
    thrd_join(th, NULL);
    close(fs.listen_fd);
}

// A server that closes without sending anything must yield NULL, not a crash.
static void empty_response_returns_null(void)
{
    _FakeServer fs;
    fs.response = "";
    fs.listen_fd = _listen_ephemeral(&fs.port);

    thrd_t th;
    assert(thrd_create(&th, _fake_server_main, &fs) == thrd_success);

    HttpClient * client = new(HttpClient);
    String * loc = new(String);
    call(loc, format, "http://127.0.0.1:%d/", fs.port);
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    HttpResponse * response = call(client, send_request, request);
    assert(response == NULL);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(client);
    thrd_join(th, NULL);
    close(fs.listen_fd);
}

// A refused connection must yield NULL, not a crash, and must not leak the fd.
static void connection_refused_returns_null(void)
{
    // Bind then close, so the port is almost certainly free and refused.
    int port;
    int probe = _listen_ephemeral(&port);
    close(probe);

    HttpClient * client = new(HttpClient);
    String * loc = new(String);
    call(loc, format, "http://127.0.0.1:%d/", port);
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    HttpResponse * response = call(client, send_request, request);
    assert(response == NULL);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(client);
}

int main(void)
{
    malformed_response_returns_null();
    empty_response_returns_null();
    connection_refused_returns_null();
    get_html_test();
    return 0;
}



