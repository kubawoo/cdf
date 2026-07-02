#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "../core/ooc.h"

#define EOL "\r\n"

typedef enum {
              HTTP_METHOD_UNKNOWN,
              HTTP_METHOD_OPTIONS,
              HTTP_METHOD_GET,
              HTTP_METHOD_HEAD,
              HTTP_METHOD_POST,
              HTTP_METHOD_PUT,
              HTTP_METHOD_DELETE,
              HTTP_METHOD_TRACE,
              HTTP_METHOD_CONNECT,
              HTTP_METHOD_PATCH
             } HttpMethod;

typedef enum {
              HTTP_STATUS_UNKNOWN=0,
              HTTP_STATUS_CONTINUE=100,
              HTTP_STATUS_SWITCHING_PROTOCOLS=101,
              HTTP_STATUS_OK=200,
              HTTP_STATUS_CREATED=201,
              HTTP_STATUS_ACCEPTED=202,
              HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION=203,
              HTTP_STATUS_NO_CONTENT=204,
              HTTP_STATUS_RESET_CONTENT=205,
              HTTP_STATUS_PARTIAL_CONTENT=206,
              HTTP_STATUS_MULTIPLE_CHOICES=300,
              HTTP_STATUS_MOVED_PERMANENTLY=301,
              HTTP_STATUS_FOUND=302,
              HTTP_STATUS_SEE_OTHER=303,
              HTTP_STATUS_NOT_MODIFIED=304,
              HTTP_STATUS_USE_PROXY=305,
              HTTP_STATUS_TEMPORARY_REDIRECT=307,
              HTTP_STATUS_BAD_REQUEST=400,
              HTTP_STATUS_UNAUTHORIZED=401,
              HTTP_STATUS_PAYMENT_REQUIRED=402,
              HTTP_STATUS_FORBIDDEN=403,
              HTTP_STATUS_NOT_FOUND=404,
              HTTP_STATUS_METHOD_NOT_ALLOWED=405,
              HTTP_STATUS_NOT_ACCEPTABLE=406,
              HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED=407,
              HTTP_STATUS_REQUEST_TIMEOUT=408,
              HTTP_STATUS_CONFLICT=409,
              HTTP_STATUS_GONE=410,
              HTTP_STATUS_LENGTH_REQUIRED=411,
              HTTP_STATUS_PRECONDITION_FAILED=412,
              HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE=413,
              HTTP_STATUS_REQUEST_URI_TOO_LONG=414,
              HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE=415,
              HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE=416,
              HTTP_STATUS_EXPECTATION_FAILED=417,
              HTTP_STATUS_INTERNAL_SERVER_ERROR=500,
              HTTP_STATUS_NOT_IMPLEMENTED=501,
              HTTP_STATUS_BAD_GATEWAY=502,
              HTTP_STATUS_SERVICE_UNAVAILABLE=503,
              HTTP_STATUS_GATEWAY_TIMEOUT=504,
              HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED=505
             } HttpStatus;

typedef struct {
    inherits(Object);
    String * (*to_string)(ObjectPtr);

    String * name;
    String * value;

} HttpHeader;

HttpHeader * HttpHeader_new2(HttpHeader *, String *, String *);
void HttpHeader_delete(ObjectPtr);


typedef struct {
    inherits(Object);
    String * (*to_string)(ObjectPtr);

    HttpMethod method;
    long content_length;
    String * path;
    String * schema;
    String * host;
    Integer * port;
    List * headers;
    String * content;
    Map * query_parameters;
    String * query_string;

    void (*add_header)(ObjectPtr, HttpHeader *);
    void (*append_content)(ObjectPtr, String *);

} HttpRequest;

HttpRequest * HttpRequest_new(HttpRequest *);
HttpRequest * HttpRequest_new2(HttpRequest *, HttpMethod, String * location);
void HttpRequest_delete(ObjectPtr);


typedef struct {
    inherits(Object);
    String * (*to_string)(ObjectPtr);

    HttpStatus status;
    List * headers;
    String * content;

    void (*add_header)(ObjectPtr, HttpHeader *);
    void (*append_content)(ObjectPtr, String *);
} HttpResponse;

HttpResponse * HttpResponse_new(HttpResponse *);
HttpResponse * HttpResponse_new1(HttpResponse *, HttpStatus);
void HttpResponse_delete(ObjectPtr);

#endif
