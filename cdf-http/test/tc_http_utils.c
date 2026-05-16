#include "test_framework.h"
#include "http_utils.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

void send_all_test(TEST_CASE_ARGUMENTS) {
    int sv[2];
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    ASSERT_TRUE(ret == 0);

    const char * msg = "hello world";
    bool ok = _http_send_all(sv[0], msg, strlen(msg));
    ASSERT_TRUE(ok);

    char buf[64] = {0};
    int n = read(sv[1], buf, sizeof(buf) - 1);
    ASSERT_TRUE(n > 0);
    ASSERT_STRINGS_EQUAL(buf, msg);

    close(sv[0]);
    close(sv[1]);
}

TEST_CASES_BEGIN
    TEST_CASE(send_all_test);
TEST_CASES_END
