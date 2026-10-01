#include "http_types.h"
#include <stdio.h>

String * HttpHeader_to_string(ObjectPtr _this) {
    make_this(HttpHeader, _this);
    String * s = new(String);
    call(s, append, this->name);
    call(s, append_cstring, ": ");
    call(s, append, this->value);
    return s;
}

HttpHeader * HttpHeader_new2(HttpHeader * this, String * name, String * value) {
    super(Object, HttpHeader);
    override(Object, to_string, HttpHeader_to_string);
    REFCINC(name);
    REFCINC(value);
    this->name = name;
    this->value = value;
    return this;
}

void HttpHeader_delete(ObjectPtr _this) {
    make_this(HttpHeader, _this);
    REFCDEC(this->name);
    REFCDEC(this->value);
    super_delete(Object, _this);
}

const char * HttpRequest_method_to_string(HttpMethod method) {
    const char * s = NULL;
    switch(method) {
    case HTTP_METHOD_OPTIONS:
        s = "OPTIONS";
        break;
    case HTTP_METHOD_GET:
        s = "GET";
        break;
    case HTTP_METHOD_HEAD:
        s = "HEAD";
        break;
    case HTTP_METHOD_POST:
        s = "POST";
        break;
    case HTTP_METHOD_PUT:
        s = "PUT";
        break;
    case HTTP_METHOD_DELETE:
        s = "DELETE";
        break;
    case HTTP_METHOD_TRACE:
        s = "TRACE";
        break;
    case HTTP_METHOD_CONNECT:
        s = "CONNECT";
        break;
    case HTTP_METHOD_PATCH:
        s = "PATCH";
        break;
    default:
       break;
    }
    return s;
}

/* Whether the caller already supplied a Content-Length header, which must not
   be duplicated or contradicted. */
static bool _HttpRequest_has_content_length(HttpRequest * this) {
    for(int i = 0; i < call(this->headers, size); ++i) {
        HttpHeader * header = (HttpHeader *) call(this->headers, get, i);
        bool match = call(header->name, equals_cstring, "Content-Length");
        REFCDEC(header);
        if(match) {
            return true;
        }
    }
    return false;
}

String * HttpRequest_to_string(ObjectPtr _this) {
    make_this(HttpRequest, _this);
    String * s = new(String);
    // method and path are NULL until the request has been parsed or given a
    // location, so both must be defaulted rather than dereferenced.
    const char * method = HttpRequest_method_to_string(this->method);
    call(s, append_cstring, method != NULL ? method : "UNKNOWN");
    call(s, append_char, ' ');
    if(this->path != NULL) {
        call(s, append, this->path);
    } else {
        call(s, append_cstring, "/");
    }
    call(s, append_cstring, " HTTP/1.1");
    call(s, append_cstring, EOL);
    for(int i = 0; i < call(this->headers, size); ++i) {
        HttpHeader * header = (HttpHeader *) call(this->headers, get, i);
        String * header_string = call(header, to_string);
        REFCDEC(header);
        call(s, append, header_string);
        REFCDEC(header_string);
        call(s, append_cstring, EOL);
    }
    /* A body with no length is unreadable to the receiver: HTTP/1.1 delimits
       it by Content-Length or by chunked framing, and this client sends
       neither. A length the caller set explicitly takes precedence. */
    if(this->content->length > 0 && !_HttpRequest_has_content_length(this)) {
        // Built separately because format() clears the string it is called on.
        String * length = new(String);
        call(length, format, "Content-Length: %zu", (size_t) this->content->length);
        call(s, append, length);
        call(s, append_cstring, EOL);
        REFCDEC(length);
    }
    call(s, append_cstring, EOL);

    if(this->content->length > 0) {
        call(s, append, this->content);
    }

    return s;
}

void HttpRequest_add_header(ObjectPtr _this, HttpHeader * header) {
    make_this(HttpRequest, _this);
    call(this->headers, add, (Object *)header);
}


String * _HttpRequest_schema_from_location(String * location) {
    int pos = call(location, index_of_cstring, "://");
    if(pos < 0) {
        //assume http
        return new(String, "http");
    }

    return call(location, substring, 0, pos);
}

/*
 * Locates the host portion of a location string.
 * Returns the offset just past "://", or 0 when no scheme is present.
 */
static int _HttpRequest_host_start(String * location) {
    int scheme = call(location, index_of_cstring, "://");
    if(scheme < 0) {
        return 0;
    }
    return scheme + 3;
}

/*
 * Locates the end of the host portion. The host is delimited by the first
 * '/', '?' or end of string; only a ':' inside that span starts a port.
 * This keeps a colon in the path (e.g. "/pa:th") from being read as a port.
 */
static int _HttpRequest_host_stop(String * location, int host_start) {
    int limit = (int) location->length;

    int slash = call(location, next_index_of_char, host_start, '/');
    if(slash >= 0 && slash < limit) {
        limit = slash;
    }

    int question = call(location, next_index_of_char, host_start, '?');
    if(question >= 0 && question < limit) {
        limit = question;
    }

    int colon = call(location, next_index_of_char, host_start, ':');
    if(colon >= 0 && colon < limit) {
        return colon;
    }

    return limit;
}

String * _HttpRequest_host_from_location(String * location) {
    int host_start = _HttpRequest_host_start(location);
    int host_stop = _HttpRequest_host_stop(location, host_start);
    if(host_stop < host_start) {
        host_stop = host_start;
    }
    return call(location, substring, host_start, host_stop);
}

Integer * _HttpRequest_port_from_location(String * location) {
    int host_start = _HttpRequest_host_start(location);
    int colon = _HttpRequest_host_stop(location, host_start);
    int limit = (int) location->length;

    int slash = call(location, next_index_of_char, host_start, '/');
    if(slash >= 0 && slash < limit) {
        limit = slash;
    }

    int question = call(location, next_index_of_char, host_start, '?');
    if(question >= 0 && question < limit) {
        limit = question;
    }

    // _HttpRequest_host_stop returns the colon only when it is within the host.
    if(colon < host_start || colon >= limit) {
        return new(Integer, 80);
    }

    int port_start = colon + 1;
    int port_stop = limit;
    if(port_start >= port_stop) {
        // "host:" with nothing after it
        return new(Integer, 80);
    }

    String * port_string = call(location, substring, port_start, port_stop);
    Integer * port = new(Integer);
    call(port, from_string, port_string);
    REFCDEC(port_string);
    return port;
}

String * _HttpRequest_path_from_location(String * location) {
    int host_start = _HttpRequest_host_start(location);

    int path_start = -1;
    int slash = call(location, next_index_of_char, host_start, '/');
    int question = call(location, next_index_of_char, host_start, '?');
    if(slash >= 0 && (question < 0 || slash < question)) {
        path_start = slash;
    } else if(question >= 0) {
        return new(String, "/");
    }

    if(path_start < 0) {
        return new(String, "/");
    }

    if(question > path_start) {
        return call(location, substring, path_start, question);
    }
    return call(location, substring_from, path_start);
}

String * _HttpRequest_query_from_location(String * location) {
    int pos = call(location, index_of_char, '?');
    if(pos > 0) {
        return call(location, substring_from, pos + 1);
    }
    return NULL;
}

Map * _HttpRequest_parse_query_string(String * query_string) {
    if(!query_string) {
        return NULL;
    }
    Map * params = new(Map);
    StringTokenizer * st = new(StringTokenizer, query_string);
    List * list = call(st, split_by_char, '&');
    REFCDEC(st);
    for(int i = 0; i < call(list, size); ++i) {
        String * pair = call(list, get, i);
        StringTokenizer * st = new(StringTokenizer, pair);
        List * pair_list = call(st, split_by_char, '=');
        REFCDEC(st);
        if(call(pair_list, size) == 2) {
            ObjectPtr pair0 = call(pair_list, get, 0);
            ObjectPtr pair1 = call(pair_list, get, 1);
            call(params, put, pair0, pair1);
            REFCDEC(pair0);
            REFCDEC(pair1);
        } else if(call(pair_list, size) == 1) {
            ObjectPtr pair0 = call(pair_list, get, 0);
            call(params, put, pair0, NULL);
            REFCDEC(pair0);
        } else {
            fprintf(stderr, "Invalid pair_list length for query params %d [%s]\n", call(pair_list, size), call(pair, to_cstring));
        }
        REFCDEC(pair);
        REFCDEC(pair_list);
    }
    REFCDEC(list);
    return params;
}

void HttpRequest_append_content(ObjectPtr _this, String * content) {
    make_this(HttpRequest, _this);
    call(this->content, append, content);
}

HttpRequest * HttpRequest_new2(HttpRequest * this, HttpMethod method, String * location) {
    super(Object, HttpRequest);
    override(Object, to_string, HttpRequest_to_string);
    this->add_header = HttpRequest_add_header;
    this->append_content = HttpRequest_append_content;
    this->method = method;
    this->headers = new(List);
    this->content = new(String);
    this->query_parameters = NULL;
    this->schema = NULL;
    this->host = NULL;
    this->port = NULL;
    this->path = NULL;
    this->query_string = NULL;

    if(location) {
        this->schema = _HttpRequest_schema_from_location(location);
        this->host = _HttpRequest_host_from_location(location);
        this->port = _HttpRequest_port_from_location(location);
        this->path = _HttpRequest_path_from_location(location);
        this->query_string = _HttpRequest_query_from_location(location);
        this->query_parameters = _HttpRequest_parse_query_string(this->query_string);


        String *host_port;
        if (this->port->value != 80) {
            String *port_string = call(this->port, to_string);
            host_port = REFCTMP(new(String, call(this->host, to_cstring)));
            call(host_port, append_char, ':');
            call(host_port, append, port_string);
            REFCDEC(port_string);
        } else {
            host_port = this->host;
        }

        call(this, add_header, REFCTMP(new(HttpHeader, REFCTMP(new(String, "Host")), host_port)));
    }

    return this;
}

HttpRequest * HttpRequest_new(HttpRequest * this) {
    this(HttpRequest, HTTP_METHOD_UNKNOWN, NULL);
    return this;
}


void HttpRequest_delete(ObjectPtr _this) {
    make_this(HttpRequest, _this);
    REFCDEC(this->schema);
    REFCDEC(this->host);
    REFCDEC(this->path);
    REFCDEC(this->port);
    REFCDEC(this->headers);
    REFCDEC(this->content);
    REFCDEC(this->query_parameters);
    REFCDEC(this->query_string);
    super_delete(Object, _this);
}

const char * _HttpResponse_status_code_with_text(HttpStatus status) {
    switch(status) {
    case HTTP_STATUS_CONTINUE:
        return "100 Continue";
    case HTTP_STATUS_SWITCHING_PROTOCOLS:
        return "101 Switching Protocols";
    case HTTP_STATUS_OK:
        return "200 OK";
    case HTTP_STATUS_CREATED:
        return "201 Created";
    case HTTP_STATUS_ACCEPTED:
        return "202 Accepted";
    case HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION:
        return "203 Non Authoritative information";
    case HTTP_STATUS_NO_CONTENT:
        return "204 No Content";
    case HTTP_STATUS_RESET_CONTENT:
        return "205 Reset Content";
    case HTTP_STATUS_PARTIAL_CONTENT:
        return "206 Partial Content";
    case HTTP_STATUS_MULTIPLE_CHOICES:
        return "300 Multiple Choices";
    case HTTP_STATUS_MOVED_PERMANENTLY:
        return "301 Moved Permanently";
    case HTTP_STATUS_FOUND:
        return "302 Found";
    case HTTP_STATUS_SEE_OTHER:
        return "303 See Other";
    case HTTP_STATUS_NOT_MODIFIED:
        return "304 Not Modified";
    case HTTP_STATUS_USE_PROXY:
        return "305 Use Proxy";
    case HTTP_STATUS_TEMPORARY_REDIRECT:
        return "307, Temporary Redirect";
    case HTTP_STATUS_BAD_REQUEST:
        return "400 Bad Request";
    case HTTP_STATUS_UNAUTHORIZED:
        return "401 Unauthorized";
    case HTTP_STATUS_PAYMENT_REQUIRED:
        return "402 Payment Required";
    case HTTP_STATUS_FORBIDDEN:
        return "403 Forbidden";
    case HTTP_STATUS_NOT_FOUND:
        return "404 Not Found";
    case HTTP_STATUS_METHOD_NOT_ALLOWED:
        return "405 Method Not Allowed";
    case HTTP_STATUS_NOT_ACCEPTABLE:
        return "406 Not Acceptable";
    case HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED:
        return "407 Proxy Authentication Required";
    case HTTP_STATUS_REQUEST_TIMEOUT:
        return "408 Request Timeout";
    case HTTP_STATUS_CONFLICT:
        return "409 Conflict";
    case HTTP_STATUS_GONE:
        return "410 Gone";
    case HTTP_STATUS_LENGTH_REQUIRED:
        return "411 Length Required";
    case HTTP_STATUS_PRECONDITION_FAILED:
        return "412 Precondition Failed";
    case HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE:
        return "413 Request Entity Too Large";
    case HTTP_STATUS_REQUEST_URI_TOO_LONG:
        return "414 Request URI Too Long";
    case HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE:
        return "415 Unsupported Media Type";
    case HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE:
        return "416 Requested Range Not Satisfiable";
    case HTTP_STATUS_EXPECTATION_FAILED:
        return "417 Expectation Failed";
    case HTTP_STATUS_UNKNOWN:
    case HTTP_STATUS_INTERNAL_SERVER_ERROR:
        return "500 Internal Server Error";
    case HTTP_STATUS_NOT_IMPLEMENTED:
        return "501 Not implemented";
    case HTTP_STATUS_BAD_GATEWAY:
        return "502 Bad Gateway";
    case HTTP_STATUS_SERVICE_UNAVAILABLE:
        return "503 Service Unavailable";
    case HTTP_STATUS_GATEWAY_TIMEOUT:
        return "504 Gateway Timeout";
    case HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED:
        return "505 HTTP Version Not Supported";
    }
    return "";
}

String * HttpResponse_to_string(ObjectPtr _this) {
    make_this(HttpResponse, _this);
    String * s = new(String, "HTTP/1.1 ");
    call(s, append_cstring, _HttpResponse_status_code_with_text(this->status));
    call(s, append_cstring, EOL);
    for(int i = 0; i < call(this->headers, size); ++i) {
        HttpHeader * header = (HttpHeader *) call(this->headers, get, i);
        String * header_string = call(header, to_string);
        REFCDEC(header);
        call(s, append, header_string);
        REFCDEC(header_string);
        call(s, append_cstring, EOL);
    }
    call(s, append_cstring, EOL);
    if(this->content->length > 0) {
        call(s, append, this->content);
    }
    return s;
}

void HttpResponse_add_header(ObjectPtr _this, HttpHeader * header) {
    make_this(HttpResponse, _this);
    call(this->headers, add, (Object *)header);
}

void HttpResponse_append_content(ObjectPtr _this, String * content) {
    make_this(HttpResponse, _this);
    call(this->content, append, content);
}

HttpResponse * HttpResponse_new(HttpResponse * this) {
    super(Object, HttpResponse);
    override(Object, to_string, HttpResponse_to_string);
    this->status = HTTP_STATUS_OK;
    this->headers = new(List);
    this->content = new(String);
    this->add_header = HttpResponse_add_header;
    this->append_content = HttpResponse_append_content;
    return this;
}

HttpResponse * HttpResponse_new1(HttpResponse * this, HttpStatus status) {
    this(HttpResponse);
    this->status = status;
    return this;
}

void HttpResponse_delete(ObjectPtr _this) {
    make_this(HttpResponse, _this);
    REFCDEC(this->content);
    REFCDEC(this->headers);
    super_delete(Object, _this);
}

