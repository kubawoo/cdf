#include "http_server.h"
#include "http_utils.h"
#include "ooc_datetime.h"
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
    if(call(s, equals_cstring, "GET")) {
        return HTTP_METHOD_GET;
    }
    if(call(s, equals_cstring, "POST")) {
        return HTTP_METHOD_POST;
    }
    if(call(s, equals_cstring, "PUT")) {
        return HTTP_METHOD_PUT;
    }
    if(call(s, equals_cstring, "DELETE")) {
        return HTTP_METHOD_DELETE;
    }
    if(call(s, equals_cstring, "HEAD")) {
        return HTTP_METHOD_HEAD;
    }
    if(call(s, equals_cstring, "OPTIONS")) {
        return HTTP_METHOD_OPTIONS;
    }
    if(call(s, equals_cstring, "TRACE")) {
        return HTTP_METHOD_TRACE;
    }
    if(call(s, equals_cstring, "CONNECT")) {
        return HTTP_METHOD_CONNECT;
    }
    if(call(s, equals_cstring, "PATCH")) {
        return HTTP_METHOD_PATCH;
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
    delete(method_string);
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
        delete(name);
        delete(value);
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
             delete(status_line);
             if(!ok) {
                 return false;
             }

             String * buffer_tmp = call(buffer, substring_from, eol_pos + 2 /* strlen(EOL) */);
             call(buffer, set_text, call(buffer_tmp, to_cstring));
             delete(buffer_tmp);
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
            delete(header_line);
            if(!ok) {
                return false;
            }

            prev_pos = pos + 2;
        }

        String * buffer_tmp = call(buffer, substring_from, prev_pos);
        call(buffer, set_text, call(buffer_tmp, to_cstring));
        delete(buffer_tmp);
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
    int buffer_len = 511;
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
        delete(s);
        delete(request);
        return NULL;
    }

    delete(s);

    if(last_bytes_read < 0) {
        fprintf(stderr, "error while reading from socket: %s\n", strerror(errno));
        delete(request);
        return NULL;
    }

    return request;
}

static int connection_main(void* _conn) {
    _ConnectionData * conn = (_ConnectionData *) _conn;
    HttpRequest * request = _parse_request(conn->conn_fd);
    if(request == NULL) {
        close(conn->conn_fd);
        delete(conn);
        return 0;
    }

    HttpResponse * response = new(HttpResponse);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Server")), REFCTMP(new(String, "CDF Server")))));
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Connection")), REFCTMP(new(String, "close")))));
    DateTime * now = new(DateTime);
    call(response, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Date")), REFCTMP(call(now, format, "%a, %d %b %Y %H:%M:%S")))));
    delete(now);

    call(conn->handler, handle, request, response);
    delete(request);
    if(response->content->length > 0) {
        Integer * content_length = new(Integer, response->content->length);
        HttpHeader * header = REFCTMP(new(HttpHeader, REFCTMP(new(String, "Content-Length")), REFCTMP(call(content_length, to_string))));
        delete(content_length);
        call(response, add_header, header);
    }
    String * response_string = call(response, to_string);
    int len = response_string->length;
    const void * data = call(response_string, to_cstring);
    _http_send_all(conn->conn_fd, data, len);

    delete(response_string);
    delete(response);
    close(conn->conn_fd);
    delete(conn);
    return 0;
}

static int server_main(void * _this) {
    make_this(HttpServer, _this);
    struct pollfd fds[1];
    memset(fds, 0 , sizeof(fds));
    fds[0].fd = this->server_socket;
    fds[0].events = POLLIN;

    while(HttpServer_is_running(this)) {
        int ret = poll(fds, 1, 1000);
        if(ret < 0) {
            fprintf(stderr, "poll failed: %s\n", strerror(errno));
            continue;
        }

        if(ret == 0) {
            //fprintf(stderr, "poll timedout\n");
            continue;
        }

        if(fds[0].revents != POLLIN) {
            fprintf(stderr, "unexpected event found: %d\n", fds[0].revents);
            continue;
        }

        int conn = accept(this->server_socket, NULL, NULL);
        if(conn < 0) {
            fprintf(stderr, "no connection: %s\n", strerror(errno));
        } else {
            //fprintf(stderr, "got connection\n");
            _ConnectionData * c = new(_ConnectionData, conn, this->_handler);
            thrd_t thread_id;
            thrd_create(&thread_id, &connection_main, c);
            thrd_detach(thread_id);
        }
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

        int flags;
        if ((flags = fcntl(this->server_socket, F_GETFL, 0)) < 0) {
            fprintf(stderr, "failed to get server socket flags: %s\n", strerror(errno));
            return;
        }
        if (fcntl(this->server_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
            fprintf(stderr, "failed to set server socket flags: %s\n", strerror(errno));
            return;
        }

        _set_run(this, true);
        thrd_create(&this->server_thread, &server_main, _this);
    }
}

static void HttpServer_stop(ObjectPtr _this) {
    make_this(HttpServer, _this);
    if(HttpServer_is_running(this)) {
        _set_run(this, false);
        thrd_join(this->server_thread, NULL);
        close(this->server_socket);
    }
}

HttpServer * HttpServer_new2(HttpServer * this, int port, HttpRequestHandler * handler) {
    super(Object, HttpServer);
    this->start = HttpServer_start;
    this->stop = HttpServer_stop;
    mtx_init(&this->run_mutex, mtx_plain);
    this->run = false;
    this->port = port;
    if(handler == NULL) {
        this->_handler = (HttpRequestHandler *) new(DummyRequestHandler);
    } else {
        this->_handler = handler;
    }
    this->is_running = HttpServer_is_running;
    return this;
}


void HttpServer_delete(ObjectPtr _this) {
    make_this(HttpServer, _this);
    mtx_destroy(&this->run_mutex);
    REFCDEC(this->_handler);
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

_ConnectionData * _ConnectionData_new2(_ConnectionData * this, int fd, HttpRequestHandler * handler) {
    super(Object, _ConnectionData);
    this->conn_fd = fd;
    this->handler = handler;
    return this;
}

void _ConnectionData_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

