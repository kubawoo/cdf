#include <string.h>
#include <stdio.h>
#include <cdf.h>
#include "cmd_create.h"
#include "cmd_build.h"
#include "cmd_test.h"

static void print_usage(void) {
    Console * c = new(Console);
    call(c, print_cstring, "Usage: cdftk <command> [args]\n\n");
    call(c, print_cstring, "Commands:\n");
    call(c, print_cstring, "  create-new-project <name>   Create a new CDF project\n");
    call(c, print_cstring, "  build                       Build the CDF project in current directory\n");
    call(c, print_cstring, "  test                        Run tests for the CDF project in current directory\n");
    REFCDEC(c);
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "create-new-project") == 0) {
        if (argc < 3) {
            Console * c = new(Console);
            call(c, print_cstring, "Usage: cdftk create-new-project <project-name>\n");
            REFCDEC(c);
            return 1;
        }
        return cmd_create_new_project(argv[2]);
    }

    if (strcmp(argv[1], "build") == 0) {
        return cmd_build();
    }

    if (strcmp(argv[1], "test") == 0) {
        return cmd_test();
    }

    Console * c = new(Console);
    call(c, print_cstring, "Unknown command\n");
    REFCDEC(c);
    return 1;
}
