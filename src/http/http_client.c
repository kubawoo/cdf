#include "http_client.h"
#include "http_utils.h"
#include "../log/log.h"
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

typedef enum {_PARSING_STATUS_LINE, _PARSING_HEADERS, _PARSING_BODY, _PARSING_CHUNKED} _HttpClient_BodyState;

typedef struct {
    _HttpClient_BodyState state;
    /* Set once the headers say the body is chunked, so the body bytes are
       de-framed rather than taken verbatim. */
    bool chunked;
    /* Set when the terminating zero-length chunk has been consumed. */
    bool complete;
} _HttpClient_ParsingState;

HttpStatus _status_from_string(String * s) {
    int code = atoi(call(s, to_cstring));
    switch(code) {
        case 100: return HTTP_STATUS_CONTINUE;
        case 101: return HTTP_STATUS_SWITCHING_PROTOCOLS;
        case 200: return HTTP_STATUS_OK;
        case 201: return HTTP_STATUS_CREATED;
        case 202: return HTTP_STATUS_ACCEPTED;
        case 203: return HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION;
        case 204: return HTTP_STATUS_NO_CONTENT;
        case 205: return HTTP_STATUS_RESET_CONTENT;
        case 206: return HTTP_STATUS_PARTIAL_CONTENT;
        case 300: return HTTP_STATUS_MULTIPLE_CHOICES;
        case 301: return HTTP_STATUS_MOVED_PERMANENTLY;
        case 302: return HTTP_STATUS_FOUND;
        case 303: return HTTP_STATUS_SEE_OTHER;
        case 304: return HTTP_STATUS_NOT_MODIFIED;
        case 305: return HTTP_STATUS_USE_PROXY;
        case 307: return HTTP_STATUS_TEMPORARY_REDIRECT;
        case 400: return HTTP_STATUS_BAD_REQUEST;
        case 401: return HTTP_STATUS_UNAUTHORIZED;
        case 402: return HTTP_STATUS_PAYMENT_REQUIRED;
        case 403: return HTTP_STATUS_FORBIDDEN;
        case 404: return HTTP_STATUS_NOT_FOUND;
        case 405: return HTTP_STATUS_METHOD_NOT_ALLOWED;
        case 406: return HTTP_STATUS_NOT_ACCEPTABLE;
        case 407: return HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED;
        case 408: return HTTP_STATUS_REQUEST_TIMEOUT;
        case 409: return HTTP_STATUS_CONFLICT;
        case 410: return HTTP_STATUS_GONE;
        case 411: return HTTP_STATUS_LENGTH_REQUIRED;
        case 412: return HTTP_STATUS_PRECONDITION_FAILED;
        case 413: return HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE;
        case 414: return HTTP_STATUS_REQUEST_URI_TOO_LONG;
        case 415: return HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE;
        case 416: return HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE;
        case 417: return HTTP_STATUS_EXPECTATION_FAILED;
        case 500: return HTTP_STATUS_INTERNAL_SERVER_ERROR;
        case 501: return HTTP_STATUS_NOT_IMPLEMENTED;
        case 502: return HTTP_STATUS_BAD_GATEWAY;
        case 503: return HTTP_STATUS_SERVICE_UNAVAILABLE;
        case 504: return HTTP_STATUS_GATEWAY_TIMEOUT;
        case 505: return HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED;
        default: return HTTP_STATUS_UNKNOWN;
    }
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
    REFCDEC(status_code_string);
    if(status == HTTP_STATUS_UNKNOWN) {
        return false;
    }

    response->status = status;
    return true;
}

bool _HttpClient_process_header_line(String * header_line, HttpResponse * response, _HttpClient_ParsingState * state) {
    int pos = call(header_line, index_of_char, ':');
    if(pos < 0) {
        return false;
    }
    // The space after the colon is optional (RFC 7230), so skip any run of
    // whitespace rather than assuming exactly one character.
    int value_start = pos + 1;
    while(value_start < (int) header_line->length &&
          isspace((unsigned char) header_line->_content[value_start])) {
        value_start++;
    }
    String * name = call(header_line, substring, 0, pos);
    String * value = call(header_line, substring_from, value_start);
    if(name->length <= 0) {
        REFCDEC(name);
        REFCDEC(value);
        return false;
    }

    /* Remember the framing while the header is still alive, since by the time
       the blank line arrives the name and value are gone. Only "chunked" needs
       de-framing; an identity encoding needs none. */
    if(call(name, equals_cstring, "Transfer-Encoding") && call(value, equals_cstring, "chunked")) {
        state->chunked = true;
    }

    HttpHeader * header = new(HttpHeader, name, value);
    REFCDEC(name);
    REFCDEC(value);
    call(response, add_header, header);
    REFCDEC(header);
    return true;
}


/* Consumes chunk framing from buffer and appends the decoded payload bytes to
   response. Anything left over — a partial size line, or a chunk whose body has
   not all arrived — stays in buffer for the next read. Returns false only when
   the framing is malformed beyond recovery. */
static bool _process_chunked(String * buffer, HttpResponse * response, _HttpClient_ParsingState * state) {
    const char * buf = call(buffer, to_cstring);
    int len = (int) buffer->length;
    int consumed = 0;

    while(!state->complete) {
        /* Chunk size line: hexadecimal digits, then CRLF. Extensions after a
           ';' are permitted (RFC 7230 §4.1) and are skipped. */
        int eol = -1;
        for(int i = consumed; i + 1 < len; ++i) {
            if(buf[i] == '\r' && buf[i + 1] == '\n') {
                eol = i;
                break;
            }
        }
        if(eol < 0) {
            break;                                  /* size line not complete yet */
        }

        long size = strtol(buf + consumed, NULL, 16);
        if(size < 0) {
            return false;                           /* malformed size line */
        }
        /* A size line with no hex digits at all is also malformed; strtol
           would have read 0 and silently ended the body. */
        if(eol == consumed) {
            return false;
        }

        int body_start = eol + 2;
        if(size == 0) {
            /* Trailer section ends at a blank line. Everything after the size
               line is a trailer, and for our purposes there is nothing to keep. */
            state->complete = true;
            consumed = body_start;
            break;
        }

        /* Each chunk body is followed by its own CRLF, so wait until the body
           plus those two bytes have arrived before appending. */
        if(body_start + size + 2 > len) {
            break;
        }
        String * payload = call(buffer, substring, body_start, body_start + (int) size);
        call(response, append_content, payload);
        REFCDEC(payload);
        consumed = body_start + (int) size + 2;
    }

    if(consumed > 0) {
        String * rest = call(buffer, substring_from, consumed);
        call(buffer, set_text, call(rest, to_cstring));
        REFCDEC(rest);
    }
    return true;
}

static bool _process_buffer(String * buffer, HttpResponse * response, _HttpClient_ParsingState * state) {

    if(state->state == _PARSING_STATUS_LINE) {
         int eol_pos = call(buffer, index_of_cstring, EOL);
         if(eol_pos < 0) {
             if(buffer->length >= 1024) {
                 //no EOL in first 1024 bytes of response - response must be invalid...
                 return false;
             }
         } else {
             String * status_line = call(buffer, substring, 0, eol_pos);
             bool ok = _HttpClient_parse_status_line(status_line, response);
             REFCDEC(status_line);
             if(!ok) {
                 return false;
             }

             String * buffer_tmp = call(buffer, substring_from, eol_pos + 2 /* strlen(EOL) */);
             call(buffer, set_text, call(buffer_tmp, to_cstring));
             REFCDEC(buffer_tmp);
             state->state = _PARSING_HEADERS;
         }
    }

    if(state->state == _PARSING_HEADERS) {
        int prev_pos = 0;
        int pos;
        while((pos = call(buffer, next_index_of_cstring, prev_pos, EOL)) > 0) {
            if(prev_pos == pos) {
                prev_pos = pos + 2;
                state->state = state->chunked ? _PARSING_CHUNKED : _PARSING_BODY;
                break;
            }
            String * header_line = call(buffer, substring, prev_pos, pos);
            bool ok = _HttpClient_process_header_line(header_line, response, state);
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

    if(state->state == _PARSING_CHUNKED) {
        return _process_chunked(buffer, response, state);
    }

    if(state->state == _PARSING_BODY) {
        call(response, append_content, buffer);
        call(buffer, clear);
    }

    return true;
}


static HttpResponse * _parse_response(HttpClient * this, int conn_fd) {
    HttpResponse * response = new(HttpResponse);
    String * s = new(String);
    _HttpClient_ParsingState state = { .state = _PARSING_STATUS_LINE, .chunked = false, .complete = false };
    int last_bytes_read = 0;
    int buffer_len = 16384;
    char buffer[buffer_len + 1];
    bool parsing_ok = false;

    /* A short read only says the kernel had nothing more buffered at that
       moment, not that the response is over: a peer that streams its reply
       dribbles it out in many reads, each smaller than the buffer. Stopping at
       the first short read truncated every chunked body mid-flight. The loop
       ends on recv() returning 0, which is the peer closing the connection. */
    while((last_bytes_read = recv(conn_fd, buffer, buffer_len, 0)) > 0) {
        buffer[last_bytes_read] = '\0';
        call(s, append_cstring, buffer);
        parsing_ok = _process_buffer(s, response, &state);
        if(!parsing_ok) {
            break;
        }
        /* A chunked body ends at the zero-length chunk, so there is no need to
           wait for the peer to close the connection. */
        if(state.state == _PARSING_CHUNKED && state.complete) {
            break;
        }
    }

    if(!parsing_ok) {
        String * msg = new(String, "Unable to parse this part of response: ");
        call(msg, append, s);
        call(this->_logger, log, LOG_LEVEL_ERROR, log_msg(msg));
        REFCDEC(msg);
        REFCDEC(s);
        REFCDEC(response);
        return NULL;
    }

    REFCDEC(s);

    if(last_bytes_read < 0) {
        String * msg = new(String, "Error while reading from socket: ");
        call(msg, append_cstring, strerror(errno));
        call(this->_logger, log, LOG_LEVEL_ERROR, log_msg(msg));
        REFCDEC(msg);
        REFCDEC(response);
        return NULL;
    }

    return response;
}

static String * resolve_hostname(HttpClient * this, String * hostname) {
    struct hostent *h;

    if ((h = gethostbyname(call(hostname, to_cstring))) == NULL) {
        String * msg = new(String, "Error while resolving host [");
        call(msg, append, hostname);
        call(msg, append_cstring, "]: ");
        call(msg, append_cstring, strerror(errno));
        call(this->_logger, log, LOG_LEVEL_ERROR, log_msg(msg));
        REFCDEC(msg);
        return NULL;
    }

    struct in_addr ** addr_list = (struct in_addr **) h->h_addr_list;
    //take the first one
    if(addr_list[0] == NULL) {
        String * msg = new(String, "No IP address found for host [");
        call(msg, append, hostname);
        call(msg, append_cstring, "]");
        call(this->_logger, log, LOG_LEVEL_ERROR, log_msg(msg));
        return NULL;
    }
    String * ip = new(String, inet_ntoa(*addr_list[0]));
    return ip;
}

HttpResponse * HttpClient_send_request(ObjectPtr _this, HttpRequest * request) {
    make_this(HttpClient, _this);
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) {
        String * msg = new(String, "Socket call failed: ");
        call(msg, append_cstring, strerror(errno));
        call(this->_logger, log, LOG_LEVEL_ERROR, log_msg(msg));
        REFCDEC(msg);
        return NULL;
    }

    if(call(this->_logger, is_enabled, LOG_LEVEL_TRACE)) {
        String * msg = new(String, "Socket call successful");
        call(this->_logger, log, LOG_LEVEL_TRACE, log_msg(msg));
        REFCDEC(msg);
    }


    struct sockaddr_in server;

    String * ip = resolve_hostname(this, request->host);
    if(!ip) {
        String * msg = new(String, "Error while resolving hostname");
        call(this->_logger, log, LOG_LEVEL_ERROR, log_msg(msg));
        REFCDEC(msg);
        close(sock);
        return NULL;
    }
    server.sin_addr.s_addr = inet_addr(call(ip, to_cstring));
    server.sin_family = AF_INET;
    server.sin_port = htons(request->port->value);

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)  {
        REFCDEC(ip);

        String * msg = new(String);
        call(msg, format, "Connecting to %s:%d failed: %s", call(request->host, to_cstring), request->port->value, strerror(errno));
        call(this->_logger, log, LOG_LEVEL_ERROR, log_msg(msg));
        REFCDEC(msg);
        close(sock);

        return NULL;
    }

    String * request_string = call(request, to_string);
    int len = request_string->length;
    const void * data = call(request_string, to_cstring);
    bool ok = _http_send_all(sock, data, len);
    REFCDEC(request_string);

    if(!ok) {
        close(sock);
        return NULL;
    }
    REFCDEC(ip);
    HttpResponse * response = _parse_response(this, sock);
    close(sock);
    if(!response) {
        return NULL;
    }
    String * response_string = call(response, to_string);
    REFCDEC(response_string);
    return response;
}

HttpClient * HttpClient_new(HttpClient * this) {
    super(Object, HttpClient);
    this->send_request = HttpClient_send_request;

    LoggerFactory * lf = singleton(LoggerFactory);
    this->_logger = call(lf, get_logger_cstring, "cdf-http-client");

    return this;
}

void HttpClient_delete(ObjectPtr _this) {
    make_this(HttpClient, _this);
    REFCDEC(this->_logger);
    super_delete(Object, _this);
}

