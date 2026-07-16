#include "http_server.h"
#include "http_utils.h"
#include "datetime.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

typedef struct {
    inherits(HttpRequestHandler);
    void (*handle)(ObjectPtr, HttpRequest *, HttpResponse *);
} DummyRequestHandler;

void DummyRequestHandler_handle(ObjectPtr _this, HttpRequest * request, HttpResponse * response) {
    String * content = new(String, "Oops... it seems server works fine, but I have "
                           "absolutely no idea what I should do with your request.\n"
                           "Make sure you told me so by implementing HttpRequestHandler");
    call(response, append_content, content);
    REFCDEC(content);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Type")), REFCTMP(new(String, "text/plain")))));
}

void DummyRequestHandler_delete(ObjectPtr _this) {
    super_delete(HttpRequestHandler, _this);
}

DummyRequestHandler * DummyRequestHandler_new(DummyRequestHandler * this) {
    super(HttpRequestHandler, DummyRequestHandler);
    override(HttpRequestHandler, handle, DummyRequestHandler_handle);
    return this;
}


bool HttpServer_is_running(ObjectPtr _this) {
    make_this(HttpServer, _this);
    mtx_lock(&(this->run_mutex));
    bool run = this->run;
    mtx_unlock(&(this->run_mutex));
    return run;
}

void _set_run(HttpServer * this, bool run) {
    mtx_lock(&(this->run_mutex));
    this->run = run;
    mtx_unlock(&(this->run_mutex));
}

typedef enum {_PARSING_STATUS_LINE, _PARSING_HEADERS, _PARSING_BODY} _HttpServer_ParsingState;

static HttpMethod _method_from_string(String * s) {
    const char *str = call(s, to_cstring);
    static const struct { const char *name; HttpMethod method; } table[] = {
        {"GET", HTTP_METHOD_GET},
        {"POST", HTTP_METHOD_POST},
        {"PUT", HTTP_METHOD_PUT},
        {"DELETE", HTTP_METHOD_DELETE},
        {"HEAD", HTTP_METHOD_HEAD},
        {"OPTIONS", HTTP_METHOD_OPTIONS},
        {"TRACE", HTTP_METHOD_TRACE},
        {"CONNECT", HTTP_METHOD_CONNECT},
        {"PATCH", HTTP_METHOD_PATCH},
    };
    for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        if (strcmp(str, table[i].name) == 0)
            return table[i].method;
    }
    return HTTP_METHOD_UNKNOWN;
}

bool _parse_status_line(String * status_line, HttpRequest * request) {
    int method_end = call(status_line, index_of_char, ' ');
    if(method_end < 0) {
        return false;
    }

    String * method_string = call(status_line, substring, 0, method_end);
    HttpMethod method = _method_from_string(method_string);
    REFCDEC(method_string);
    if(method == HTTP_METHOD_UNKNOWN) {
        return false;
    }

    int location_end = call(status_line, next_index_of_char, method_end + 1, ' ');

    if(location_end < 0) {
        return false;
    }

    request->method = method;
    request->path = call(status_line, substring, method_end + 1, location_end);
    return true;
}

static bool _process_header_line(String * header_line, HttpRequest * request) {
    int pos = call(header_line, index_of_char, ':');
    if(pos < 0) {
        return false;
    }
    String * name = REFCTMP(call(header_line, substring, 0, pos));
    String * value = REFCTMP(call(header_line, substring_from, pos + 2));
    if(name->length <= 0 || value->length <= 0) {
        REFCDEC(name);
        REFCDEC(value);
        return false;
    }
    call(request, add_header, REFCTMP(new(HttpHeader, name, value)));
    return true;
}


static bool _process_buffer(String * buffer, HttpRequest * request, _HttpServer_ParsingState * state) {

    if(*state == _PARSING_STATUS_LINE) {
         int eol_pos = call(buffer, index_of_cstring, EOL);
         if(eol_pos < 0) {
             if(buffer->length >= 1024) {
                 //no EOL in first 1024 bytes of request - request must be invalid...
                 return false;
             }
         } else {
             String * status_line = call(buffer, substring, 0, eol_pos);
             bool ok = _parse_status_line(status_line, request);
             REFCDEC(status_line);
             if(!ok) {
                 return false;
             }

             String * buffer_tmp = call(buffer, substring_from, eol_pos + 2 /* strlen(EOL) */);
             call(buffer, set_text, call(buffer_tmp, to_cstring));
             REFCDEC(buffer_tmp);
             *state = _PARSING_HEADERS;
         }
    }

    if(*state == _PARSING_HEADERS) {
        int prev_pos = 0;
        int pos;
        while((pos = call(buffer, next_index_of_cstring, prev_pos, EOL)) > 0) {
            if(prev_pos == pos) {
                prev_pos = pos + 2;
                *state = _PARSING_BODY;
                break;
            }
            String * header_line = call(buffer, substring, prev_pos, pos);
            bool ok = _process_header_line(header_line, request);
            REFCDEC(header_line);
            if(!ok) {
                return false;
            }

            prev_pos = pos + 2;
        }

        String * buffer_tmp = call(buffer, substring_from, prev_pos);
        call(buffer, set_text, call(buffer_tmp, to_cstring));
        REFCDEC(buffer_tmp);
    }

    if(*state == _PARSING_BODY) {
        call(request, append_content, buffer);
        call(buffer, clear);
    }
    return true;
}


static HttpRequest * _parse_request(int conn_fd) {
    HttpRequest * request = new(HttpRequest);
    String * s = new(String);
    _HttpServer_ParsingState state = _PARSING_STATUS_LINE;
    int last_bytes_read = 0;
    int buffer_len = 16384;
    char buffer[buffer_len + 1];
    bool parsing_ok = false;

    while((last_bytes_read = recv(conn_fd, buffer, buffer_len, 0)) > 0) {
        buffer[last_bytes_read] = '\0';
        call(s, append_cstring, buffer);
        parsing_ok = _process_buffer(s, request, &state);
        if(!parsing_ok) {
            break;
        }
        if(last_bytes_read < buffer_len) {
            break;
        }
    }

    if(!parsing_ok) {
        fprintf(stderr, "Unable to parse this part of request: %s\n", call(s, to_cstring));
        REFCDEC(s);
        REFCDEC(request);
        return NULL;
    }

    REFCDEC(s);

    if(last_bytes_read < 0) {
        fprintf(stderr, "error while reading from socket: %s\n", strerror(errno));
        REFCDEC(request);
        return NULL;
    }

    return request;
}

static int connection_main(void* _conn) {
    _ConnectionData * conn = (_ConnectionData *) _conn;
    HttpServer * server = conn->_server;

    HttpRequest * request = _parse_request(conn->conn_fd);
    if(request == NULL) {
        close(conn->conn_fd);
        REFCDEC(conn);

        mtx_lock(&server->conn_mutex);
        server->_active_connections--;
        cnd_signal(&server->conn_cv);
        mtx_unlock(&server->conn_mutex);
        return 0;
    }

    HttpResponse * response = new(HttpResponse);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Server")), REFCTMP(new(String, "CDF Server")))));
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Connection")), REFCTMP(new(String, "close")))));
    time_t now_sec = time(NULL);
    if (now_sec != server->_last_date_update || !server->_cached_date) {
        DateTime * dt = new(DateTime);
        String * fmt = call(dt, format, "%a, %d %b %Y %H:%M:%S");
        REFCDEC(server->_cached_date);
        server->_cached_date = fmt;
        REFCDEC(dt);
        server->_last_date_update = now_sec;
    }
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Date")), REFCTMP(call(server->_cached_date, copy)))));

    call(conn->handler, handle, request, response);
    REFCDEC(request);
    if(response->content->length > 0) {
        Integer * content_length = new(Integer, response->content->length);
        HttpHeader * header = REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(call(content_length, to_string))));
        REFCDEC(content_length);
        call(response, add_header, header);
    }
    String * response_string = call(response, to_string);
    int len = response_string->length;
    const void * data = call(response_string, to_cstring);
    _http_send_all(conn->conn_fd, data, len);

    REFCDEC(response_string);
    REFCDEC(response);
    close(conn->conn_fd);
    REFCDEC(conn);

    mtx_lock(&server->conn_mutex);
    server->_active_connections--;
    cnd_signal(&server->conn_cv);
    mtx_unlock(&server->conn_mutex);
    return 0;
}

static int worker_main(void * arg) {
    HttpServer * server = (HttpServer *)arg;
    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = server->server_socket;
    pfd.events = POLLIN;

    while (HttpServer_is_running(server)) {
        int ret = poll(&pfd, 1, 500);
        if (ret <= 0) continue;
        int conn = accept(server->server_socket, NULL, NULL);
        if (conn < 0) continue;
        _ConnectionData * c = new(_ConnectionData, conn, server->_handler, server);
        mtx_lock(&server->conn_mutex);
        server->_active_connections++;
        mtx_unlock(&server->conn_mutex);
        connection_main(c);
    }
    return 0;
}

static int server_main(void * _this) {
    make_this(HttpServer, _this);
    for (int i = 0; i < HTTP_SERVER_POOL_SIZE; i++) {
        thrd_create(&this->_workers[i], worker_main, this);
    }
    for (int i = 0; i < HTTP_SERVER_POOL_SIZE; i++) {
        thrd_join(this->_workers[i], NULL);
    }
    return 0;
}

static void HttpServer_start(ObjectPtr _this) {
    make_this(HttpServer, _this);
    if(!HttpServer_is_running(this)) {
        this->server_socket = socket(PF_INET, SOCK_STREAM, 0);
        int optval = 1;
        setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        if(this->server_socket < 0) {
            fprintf(stderr, "failed to create server socket: %s\n", strerror(errno));
            return;
        }

        struct sockaddr_in socket_address;
        struct in_addr local_address;
        local_address.s_addr = INADDR_ANY;
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(this->port);
        socket_address.sin_addr = local_address;

        int ok = bind(this->server_socket, (struct sockaddr *) &socket_address, sizeof (socket_address));
        if (ok != 0) {
            fprintf(stderr, "failed to bind server socket: %s\n", strerror(errno));
            return;
        }
        ok = listen(this->server_socket, 10);
        if (ok != 0) {
            fprintf(stderr, "failed to listen on server socket: %s\n", strerror(errno));
            return;
        }

        _set_run(this, true);

        int flags = fcntl(this->server_socket, F_GETFL, 0);
        fcntl(this->server_socket, F_SETFL, flags | O_NONBLOCK);

        thrd_create(&this->server_thread, &server_main, _this);
    }
}

static void HttpServer_stop(ObjectPtr _this) {
    make_this(HttpServer, _this);
    if(HttpServer_is_running(this)) {
        _set_run(this, false);
        close(this->server_socket);
        thrd_join(this->server_thread, NULL);

        mtx_lock(&this->conn_mutex);
        while(this->_active_connections > 0) {
            cnd_wait(&this->conn_cv, &this->conn_mutex);
        }
        mtx_unlock(&this->conn_mutex);
    }
}

HttpServer * HttpServer_new2(HttpServer * this, int port, HttpRequestHandler * handler) {
    super(Object, HttpServer);
    this->start = HttpServer_start;
    this->stop = HttpServer_stop;
    mtx_init(&this->run_mutex, mtx_plain);
    mtx_init(&this->conn_mutex, mtx_plain);
    cnd_init(&this->conn_cv);
    this->run = false;
    this->_active_connections = 0;
    this->port = port;
    if(handler == NULL) {
        this->_handler = (HttpRequestHandler *) new(DummyRequestHandler);
    } else {
        this->_handler = handler;
    }
    this->_cached_date = NULL;
    this->_last_date_update = 0;
    this->is_running = HttpServer_is_running;
    return this;
}


void HttpServer_delete(ObjectPtr _this) {
    make_this(HttpServer, _this);
    mtx_destroy(&this->run_mutex);
    mtx_destroy(&this->conn_mutex);
    cnd_destroy(&this->conn_cv);
    REFCDEC(this->_handler);
    REFCDEC(this->_cached_date);
    super_delete(Object, _this);
}

HttpRequestHandler * HttpRequestHandler_new(HttpRequestHandler * this) {
    super(Object, HttpRequestHandler);
    this->handle = NULL;
    return this;
}

void HttpRequestHandler_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

_ConnectionData * _ConnectionData_new3(_ConnectionData * this, int fd, HttpRequestHandler * handler, HttpServer * server) {
    super(Object, _ConnectionData);
    this->conn_fd = fd;
    REFCINC(handler);
    this->handler = handler;
    this->_server = server;
    return this;
}

void _ConnectionData_delete(ObjectPtr _this) {
    make_this(_ConnectionData, _this);
    REFCDEC(this->handler);
    super_delete(Object, _this);
}

