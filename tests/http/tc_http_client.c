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
    /* When set, the server sends `first` and `second` as two separate writes
       with a pause between them, so the client sees the response arrive
       dribbled across several short reads. */
    const char * first;
    const char * second;
} _FakeServer;

static int _fake_server_main(void * arg) {
    _FakeServer * fs = (_FakeServer *) arg;
    // Serve exactly one connection, then exit, so thrd_join always returns.
    int c = accept(fs->listen_fd, NULL, NULL);
    if(c >= 0) {
        char buf[2048];
        recv(c, buf, sizeof(buf), 0);
        if(fs->first) {
            // The gap forces the client to block in recv() on the second half,
            // which is what a streaming server does.
            send(c, fs->first, strlen(fs->first), 0);
            usleep(200000);
            send(c, fs->second, strlen(fs->second), 0);
        } else if(fs->response && fs->response[0] != '\0') {
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
    _FakeServer fs = { .first = NULL, .second = NULL };
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

// Headers may omit the space after the colon (RFC 7230). The value must be
// captured whole, not lose its first character.
static void header_without_space_after_colon(void)
{
    _FakeServer fs = { .first = NULL, .second = NULL };
    fs.response = "HTTP/1.1 200 OK\r\n"
                  "Content-Type:application/json\r\n"
                  "X-Multi:   spaced   \r\n"
                  "\r\n";
    fs.listen_fd = _listen_ephemeral(&fs.port);

    thrd_t th;
    assert(thrd_create(&th, _fake_server_main, &fs) == thrd_success);

    HttpClient * client = new(HttpClient);
    String * loc = new(String);
    call(loc, format, "http://127.0.0.1:%d/", fs.port);
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    HttpResponse * response = call(client, send_request, request);
    assert(response != NULL);
    assert((response->status) == (HTTP_STATUS_OK));

    // find the Content-Type header and check nothing was dropped
    HttpHeader * found = NULL;
    HttpHeader * multi = NULL;
    for(int i = 0; i < call(response->headers, size); ++i) {
        HttpHeader * h = (HttpHeader *) call(response->headers, get, i);
        const char * nm = call(h->name, to_cstring);
        if(strcmp(nm, "Content-Type") == 0) {
            found = h;
        } else if(strcmp(nm, "X-Multi") == 0) {
            multi = h;
        } else {
            REFCDEC(h);
        }
    }
    // no space after the colon: value must be intact
    assert(found != NULL);
    assert(strcmp(call(found->value, to_cstring), "application/json") == 0);
    REFCDEC(found);

    // several spaces after the colon: all leading whitespace skipped
    assert(multi != NULL);
    assert(strcmp(call(multi->value, to_cstring), "spaced   ") == 0);
    REFCDEC(multi);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(response);
    REFCDEC(client);
    thrd_join(th, NULL);
    close(fs.listen_fd);
}

// A server that closes without sending anything must yield NULL, not a crash.
static void empty_response_returns_null(void)
{
    _FakeServer fs = { .first = NULL, .second = NULL };
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

// A header with an empty value is legal (RFC 7230) and must not make the whole
// response unreadable. llama.cpp sends "Access-Control-Allow-Origin: " on
// every response, so rejecting this made chat requests fail outright.
static void empty_header_value_is_accepted(void)
{
    _FakeServer fs = { .first = NULL, .second = NULL };
    fs.response = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: application/json\r\n"
                  "Access-Control-Allow-Origin: \r\n"
                  "Content-Length: 11\r\n"
                  "\r\n"
                  "{\"ok\":true}";
    fs.listen_fd = _listen_ephemeral(&fs.port);

    thrd_t th;
    assert(thrd_create(&th, _fake_server_main, &fs) == thrd_success);

    HttpClient * client = new(HttpClient);
    String * loc = new(String);
    call(loc, format, "http://127.0.0.1:%d/", fs.port);
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    HttpResponse * response = call(client, send_request, request);
    assert(response != NULL);
    assert((response->status) == (HTTP_STATUS_OK));
    assert(strcmp(call(response->content, to_cstring), "{\"ok\":true}") == 0);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(response);
    REFCDEC(client);
    thrd_join(th, NULL);
    close(fs.listen_fd);
}

/* Sends a raw response in two halves with a pause between them, asserting the
   decoded body equals expected. */
static void check_split_response(const char * first, const char * second, const char * expected)
{
    _FakeServer fs = { .first = first, .second = second };
    fs.response = NULL;
    fs.listen_fd = _listen_ephemeral(&fs.port);

    thrd_t th;
    assert(thrd_create(&th, _fake_server_main, &fs) == thrd_success);

    HttpClient * client = new(HttpClient);
    String * loc = new(String);
    call(loc, format, "http://127.0.0.1:%d/", fs.port);
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    HttpResponse * response = call(client, send_request, request);
    assert(response != NULL);
    assert((response->status) == (HTTP_STATUS_OK));
    assert(strcmp(call(response->content, to_cstring), expected) == 0);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(response);
    REFCDEC(client);
    thrd_join(th, NULL);
    close(fs.listen_fd);
}

/* Sends a raw response, asserting the decoded body equals expected. Every
   chunked response below is framed by hand because the framing, not the body,
   is what is under test. */
static void check_response(const char * raw, const char * expected)
{
    _FakeServer fs = { .first = NULL, .second = NULL };
    fs.response = raw;
    fs.listen_fd = _listen_ephemeral(&fs.port);

    thrd_t th;
    assert(thrd_create(&th, _fake_server_main, &fs) == thrd_success);

    HttpClient * client = new(HttpClient);
    String * loc = new(String);
    call(loc, format, "http://127.0.0.1:%d/", fs.port);
    HttpRequest * request = new(HttpRequest, HTTP_METHOD_GET, loc);

    HttpResponse * response = call(client, send_request, request);
    assert(response != NULL);
    assert((response->status) == (HTTP_STATUS_OK));
    assert(strcmp(call(response->content, to_cstring), expected) == 0);

    REFCDEC(loc);
    REFCDEC(request);
    REFCDEC(response);
    REFCDEC(client);
    thrd_join(th, NULL);
    close(fs.listen_fd);
}

// A chunked body must be de-framed, not handed back with its size lines still
// attached. llama.cpp uses this encoding for /v1/chat/completions.
static void chunked_body_is_decoded(void)
{
    // {"ok":true} is 11 bytes, which is 0xb.
    check_response(
        "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
        "Transfer-Encoding: chunked\r\n\r\n"
        "b\r\n{\"ok\":true}\r\n0\r\n\r\n",
        "{\"ok\":true}");
}

// Several chunks must be concatenated in the order they arrived.
static void chunked_body_joins_multiple_chunks(void)
{
    check_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "5\r\nHello\r\n1\r\n \r\n5\r\nWorld\r\n0\r\n\r\n",
        "Hello World");
}

// The size is hexadecimal, so 0x10 is sixteen bytes, not ten.
static void chunk_size_is_hexadecimal(void)
{
    check_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "B\r\n{\"ok\":true}\r\n0\r\n\r\n",
        "{\"ok\":true}");
}

// A chunk extension after the size must be ignored (RFC 7230 4.1).
static void chunk_extension_is_ignored(void)
{
    check_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "b;foo=bar\r\n{\"ok\":true}\r\n0\r\n\r\n",
        "{\"ok\":true}");
}

// Trailers follow the terminating chunk and are not part of the body.
static void chunk_trailers_are_discarded(void)
{
    check_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "b\r\n{\"ok\":true}\r\n0\r\nX-Trailer: v\r\n\r\n",
        "{\"ok\":true}");
}

// An immediate terminating chunk means an empty body, not a failure.
static void chunked_empty_body_is_accepted(void)
{
    check_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "0\r\n\r\n",
        "");
}

// A response declaring chunked framing but sent with an identity body must
// still be readable, so a server that mislabels its encoding is not fatal.
static void identity_body_is_not_deframed(void)
{
    check_response(
        "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
        "Content-Length: 11\r\n\r\n"
        "{\"ok\":true}",
        "{\"ok\":true}");
}

/* A response that arrives in several TCP writes must be read to the end. A
   short read only means the kernel had nothing more buffered at that instant;
   treating it as end-of-response cut every streamed reply off after the first
   packet, which is most of what llama.cpp sends back. */
static void response_split_across_packets_is_read_whole(void)
{
    check_split_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n",
        "b\r\n{\"ok\":true}\r\n0\r\n\r\n",
        "{\"ok\":true}");
}

/* The same split, but with the body itself cut across the packet boundary, so
   the second read continues a chunk that the first read left incomplete. */
static void chunk_body_split_across_packets_is_reassembled(void)
{
    check_split_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "5\r\nHello\r\n6\r\n wor",
        "ld\r\n0\r\n\r\n",
        "Hello world");
}

/* The headers themselves can straddle the boundary too. */
static void headers_split_across_packets_are_read(void)
{
    check_split_response(
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n"
        "Content-Ty",
        "pe: application/json\r\n\r\nb\r\n{\"ok\":true}\r\n0\r\n\r\n",
        "{\"ok\":true}");
}

int main(void)
{
    malformed_response_returns_null();
    header_without_space_after_colon();
    empty_response_returns_null();
    connection_refused_returns_null();
    empty_header_value_is_accepted();
    chunked_body_is_decoded();
    chunked_body_joins_multiple_chunks();
    chunk_size_is_hexadecimal();
    chunk_extension_is_ignored();
    chunk_trailers_are_discarded();
    chunked_empty_body_is_accepted();
    identity_body_is_not_deframed();
    response_split_across_packets_is_read_whole();
    chunk_body_split_across_packets_is_reassembled();
    headers_split_across_packets_are_read();
    get_html_test();
    return 0;
}



