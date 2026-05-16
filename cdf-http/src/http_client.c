#include "http_client.h"
#include "http_utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

typedef enum {_PARSING_STATUS_LINE, _PARSING_HEADERS, _PARSING_BODY} _HttpClient_ParsingState;



HttpStatus _status_from_string(String * s) {
    //TODO convert to int and make switch
    if(call(s, equals_cstring, "100")) {
        return HTTP_STATUS_CONTINUE;
    }
    if(call(s, equals_cstring, "101")) {
        return HTTP_STATUS_SWITCHING_PROTOCOLS;
    }
    if(call(s, equals_cstring, "200")) {
        return HTTP_STATUS_OK;
    }
    if(call(s, equals_cstring, "201")) {
        return HTTP_STATUS_CREATED;
    }
    if(call(s, equals_cstring, "202")) {
        return HTTP_STATUS_ACCEPTED;
    }
    if(call(s, equals_cstring, "203")) {
        return HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION;
    }
    if(call(s, equals_cstring, "204")) {
        return HTTP_STATUS_NO_CONTENT;
    }
    if(call(s, equals_cstring, "205")) {
        return HTTP_STATUS_RESET_CONTENT;
    }
    if(call(s, equals_cstring, "206")) {
        return HTTP_STATUS_PARTIAL_CONTENT;
    }
    if(call(s, equals_cstring, "300")) {
        return HTTP_STATUS_MULTIPLE_CHOICES;
    }
    if(call(s, equals_cstring, "301")) {
        return HTTP_STATUS_MOVED_PERMANENTLY;
    }
    if(call(s, equals_cstring, "302")) {
        return HTTP_STATUS_FOUND;
    }
    if(call(s, equals_cstring, "303")) {
        return HTTP_STATUS_SEE_OTHER;
    }
    if(call(s, equals_cstring, "304")) {
        return HTTP_STATUS_NOT_MODIFIED;
    }
    if(call(s, equals_cstring, "305")) {
        return HTTP_STATUS_USE_PROXY;
    }
    if(call(s, equals_cstring, "307")) {
        return HTTP_STATUS_TEMPORARY_REDIRECT;
    }
    if(call(s, equals_cstring, "400")) {
        return HTTP_STATUS_BAD_REQUEST;
    }
    if(call(s, equals_cstring, "401")) {
        return HTTP_STATUS_UNAUTHORIZED;
    }
    if(call(s, equals_cstring, "402")) {
        return HTTP_STATUS_PAYMENT_REQUIRED;
    }
    if(call(s, equals_cstring, "403")) {
        return HTTP_STATUS_FORBIDDEN;
    }
    if(call(s, equals_cstring, "404")) {
        return HTTP_STATUS_NOT_FOUND;
    }
    if(call(s, equals_cstring, "405")) {
        return HTTP_STATUS_METHOD_NOT_ALLOWED;
    }
    if(call(s, equals_cstring, "406")) {
        return HTTP_STATUS_NOT_ACCEPTABLE;
    }
    if(call(s, equals_cstring, "407")) {
        return HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED;
    }
    if(call(s, equals_cstring, "408")) {
        return HTTP_STATUS_REQUEST_TIMEOUT;
    }
    if(call(s, equals_cstring, "409")) {
        return HTTP_STATUS_CONFLICT;
    }
    if(call(s, equals_cstring, "410")) {
        return HTTP_STATUS_GONE;
    }
    if(call(s, equals_cstring, "411")) {
        return HTTP_STATUS_LENGTH_REQUIRED;
    }
    if(call(s, equals_cstring, "412")) {
        return HTTP_STATUS_PRECONDITION_FAILED;
    }
    if(call(s, equals_cstring, "413")) {
        return HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE;
    }
    if(call(s, equals_cstring, "414")) {
        return HTTP_STATUS_REQUEST_URI_TOO_LONG;
    }
    if(call(s, equals_cstring, "415")) {
        return HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE;
    }
    if(call(s, equals_cstring, "416")) {
        return HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE;
    }
    if(call(s, equals_cstring, "417")) {
        return HTTP_STATUS_EXPECTATION_FAILED;
    }
    if(call(s, equals_cstring, "500")) {
        return HTTP_STATUS_INTERNAL_SERVER_ERROR;
    }
    if(call(s, equals_cstring, "501")) {
        return HTTP_STATUS_NOT_IMPLEMENTED;
    }
    if(call(s, equals_cstring, "502")) {
        return HTTP_STATUS_BAD_GATEWAY;
    }
    if(call(s, equals_cstring, "503")) {
        return HTTP_STATUS_SERVICE_UNAVAILABLE;
    }
    if(call(s, equals_cstring, "504")) {
        return HTTP_STATUS_GATEWAY_TIMEOUT;
    }
    if(call(s, equals_cstring, "505")) {
        return HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED;
    }
    return HTTP_STATUS_UNKNOWN;
}

bool _HttpClient_parse_status_line(String * status_line, HttpResponse * response) {
    int protocol_end = call(status_line, index_of_char, ' ');
    if(protocol_end < 0) {
        return false;
    }

    int status_code_start = protocol_end + 1;
    int status_code_end = call(status_line, next_index_of_char, status_code_start, ' ');

    String * status_code_string = call(status_line, substring, status_code_start, status_code_end);
    HttpStatus status = _status_from_string(status_code_string);
    delete(status_code_string);
    if(status == HTTP_STATUS_UNKNOWN) {
        return false;
    }

    response->status = status;
    return true;
}

bool _HttpClient_process_header_line(String * header_line, HttpResponse * response) {
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
    call(response, add_header, REFCTMP(new(HttpHeader, name, value)));
    return true;
}


static bool _process_buffer(String * buffer, HttpResponse * response, _HttpClient_ParsingState * state) {

    if(*state == _PARSING_STATUS_LINE) {
         int eol_pos = call(buffer, index_of_cstring, EOL);
         if(eol_pos < 0) {
             if(buffer->length >= 1024) {
                 //no EOL in first 1024 bytes of response - response must be invalid...
                 return false;
             }
         } else {
             String * status_line = call(buffer, substring, 0, eol_pos);
             bool ok = _HttpClient_parse_status_line(status_line, response);
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
            bool ok = _HttpClient_process_header_line(header_line, response);
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
        call(response, append_content, buffer);
        call(buffer, clear);
    }

    return true;
}


static HttpResponse * _parse_response(int conn_fd) {
    HttpResponse * response = new(HttpResponse);
    String * s = new(String);
    _HttpClient_ParsingState state = _PARSING_STATUS_LINE;
    int last_bytes_read = 0;
    int buffer_len = 511;
    char buffer[buffer_len + 1];
    bool parsing_ok = false;

    while((last_bytes_read = recv(conn_fd, buffer, buffer_len, 0)) > 0) {
        buffer[last_bytes_read] = '\0';
        call(s, append_cstring, buffer);
        parsing_ok = _process_buffer(s, response, &state);
        if(!parsing_ok) {
            break;
        }
        if(last_bytes_read < buffer_len) {
            break;
        }
    }

    if(!parsing_ok) {
        fprintf(stderr, "Unable to parse this part of response: %s\n", call(s, to_cstring));
        delete(s);
        delete(response);
        return NULL;
    }

    delete(s);

    if(last_bytes_read < 0) {
        fprintf(stderr, "error while reading from socket: %s\n", strerror(errno));
        delete(response);
        return NULL;
    }

    return response;
}

static String * resolve_hostname(String * hostname) {
    struct hostent *h;

    if ((h = gethostbyname(call(hostname, to_cstring))) == NULL) {
        fprintf(stderr, "error while resolving host %s: %s\n", call(hostname, to_cstring), strerror(errno));
        return NULL;
    }

    struct in_addr ** addr_list = (struct in_addr **) h->h_addr_list;
    //take the first one
    if(addr_list[0] == NULL) {
        fprintf(stderr, "no ip found for host %s\n", call(hostname, to_cstring));
        return NULL;
    }
    String * ip = new(String, inet_ntoa(*addr_list[0]));
    return ip;
}

HttpResponse * HttpClient_send_request(ObjectPtr _this, HttpRequest * request) {
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) {
        fprintf(stderr, "socket call failed: %s\n", strerror(errno));
        return NULL;
    }

    struct sockaddr_in server;

    String * ip = resolve_hostname(request->host);
    if(!ip) {
        REFCDEC(ip);
        fprintf(stderr, "%s\n", "Error while resolving hostname");
        return NULL;
    }
    server.sin_addr.s_addr = inet_addr(call(ip, to_cstring));
    server.sin_family = AF_INET;
    server.sin_port = htons(request->port->value);

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)  {
        REFCDEC(ip);
        fprintf(stderr, "connect to %s:%d failed: %s\n", call(request->host, to_cstring), request->port->value, strerror(errno));
        return NULL;
    }

    String * request_string = call(request, to_string);
    int len = request_string->length;
    const void * data = call(request_string, to_cstring);
    bool ok = _http_send_all(sock, data, len);
    REFCDEC(request_string);

    if(!ok) {
        return NULL;
    }
    REFCDEC(ip);
    HttpResponse * response = _parse_response(sock);
    close(sock);
    String * response_string = call(response, to_string);
    fprintf(stderr, "RESPONSE: %s\n\n", call(response_string, to_cstring));
    REFCDEC(response_string);
    return response;
}

HttpClient * HttpClient_new(HttpClient * this) {
    super(Object, HttpClient);
    this->send_request = HttpClient_send_request;
    return this;
}

void HttpClient_delete(ObjectPtr _this) {
    super_delete(Object, _this);
}

