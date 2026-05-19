#ifndef HANDLERS_H
#define HANDLERS_H

#include <cdf.h>
#include <http.h>
#include <json.h>
#include <entitymanager.h>

typedef struct {
    inherits(HttpRequestHandler);
    void (*handle)(void *, HttpRequest *, HttpResponse *);
    EntityManager * em;
} TodoRequestHandler;

TodoRequestHandler * TodoRequestHandler_new(TodoRequestHandler * this);
void TodoRequestHandler_delete(ObjectPtr this);

#endif
