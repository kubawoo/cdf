#include <cdf.h>
#include <http.h>
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

int main(void) {
    signal(SIGINT, signal_handler);

    server = new(HttpServer, PORT, (HttpRequestHandler *)new(TodoRequestHandler));
    printf("Starting todo-list-api on port %d\n", PORT);
    call(server, start);

    if (call(server, is_running))
        printf("Server started\n");

    while (call(server, is_running))
        sleep(1);

    delete(server);
    printf("Bye bye\n");
    return 0;
}
