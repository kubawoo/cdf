#include <cdf.h>
#include <http.h>
#include <db_sqlite.h>
#include <entitymanager.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "handlers.h"

#define PORT 19876
HttpServer * server = NULL;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("\nStopping the server...\n");
        if (server != NULL)
            call(server, stop);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);

    Database * db;
    if (argc > 1) {
        String * path = new(String, argv[1]);
        db = new(SQLiteDatabase, path);
        REFCDEC(path);
        printf("Using database file: %s\n", argv[1]);
    } else {
        db = new(SQLiteDatabase);
        printf("Using in-memory database\n");
    }

    EntityManager * em = new(EntityManager, db);
    REFCDEC(db);

    TodoRequestHandler * handler = new(TodoRequestHandler);
    handler->em = em;
    REFCINC(em);

    server = new(HttpServer, PORT, (HttpRequestHandler *)handler);
    printf("Starting todo-list-api on port %d\n", PORT);
    call(server, start);

    if (call(server, is_running))
        printf("Server started\n");

    while (call(server, is_running))
        sleep(1);

    delete(server);
    REFCDEC(handler);
    REFCDEC(em);
    printf("Bye bye\n");
    return 0;
}
