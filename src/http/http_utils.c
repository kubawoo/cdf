#include "http_utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

bool _http_send_all(int sock, const void * data, int length) {
    int bytes_sent = 0;
    while(bytes_sent < length) {
        int i = send(sock, data + bytes_sent, length - bytes_sent, 0);
        if(i < 0) {
            fprintf(stderr, "send call failed: %s\n", strerror(errno));
            return false;
        }
        bytes_sent += i;
    }
    return true;
}