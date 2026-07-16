#include <core.h>
#include <http.h>
#include <db_sqlite.h>
#include <entitymanager.h>
#include <log.h>
#include <signal.h>
#include <unistd.h>
#include "handlers.h"

#define PORT 19876
static HttpServer * server = NULL;
static Logger * logger = NULL;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        call(logger, log, LOG_LEVEL_INFO, log_msg(REFCTMP(new(String, "Stopping the server..."))));
        if (server != NULL)
            call(server, stop);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);

    logger = new(Logger, REFCTMP(new(String, "todo-list-api")), LOG_LEVEL_DEBUG);

    Database * db;
    if (argc > 1) {
        String * path = new(String, argv[1]);
        db = new(SQLiteDatabase, path);
        REFCDEC(path);
        String * msg = new(String);
        call(msg, format, "Using database file: %s", argv[1]);
        call(logger, log, LOG_LEVEL_INFO, log_msg(msg));
        REFCDEC(msg);
    } else {
        db = new(SQLiteDatabase);
        call(logger, log, LOG_LEVEL_INFO, log_msg(REFCTMP(new(String, "Using in-memory database"))));
    }

    EntityManager * em = new(EntityManager, db);
    REFCDEC(db);

    TodoRequestHandler * handler = new(TodoRequestHandler);
    handler->em = em;
    REFCINC(em);

    server = new(HttpServer, PORT, (HttpRequestHandler *)handler);
    String * msg = new(String);
    call(msg, format, "Starting todo-list-api on port %d", PORT);
    call(logger, log, LOG_LEVEL_INFO, log_msg(msg));
    REFCDEC(msg);
    call(server, start);

    if (call(server, is_running))
        call(logger, log, LOG_LEVEL_INFO, log_msg(REFCTMP(new(String, "Server started"))));

    while (call(server, is_running))
        sleep(1);

    REFCDEC(server);
    REFCDEC(handler);
    REFCDEC(em);
    call(logger, log, LOG_LEVEL_INFO, log_msg(REFCTMP(new(String, "Bye bye"))));
    REFCDEC(logger);
    return 0;
}
