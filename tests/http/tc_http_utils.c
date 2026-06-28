#include "http_utils.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void send_all_test(void)
{
    int sv[2];
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    assert(ret == 0);

    const char * msg = "hello world";
    bool ok = _http_send_all(sv[0], msg, strlen(msg));
    assert(ok);

    char buf[64] = {0};
    int n = read(sv[1], buf, sizeof(buf) - 1);
    assert(n > 0);
    assert(strcmp(buf, msg) == 0);

    close(sv[0]);
    close(sv[1]);
}
int main(void)
{
    send_all_test();
    return 0;
}

