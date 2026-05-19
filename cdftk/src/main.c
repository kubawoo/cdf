#include <cdf.h>
#include "cmd_create.h"
#include "cmd_build.h"
#include "cmd_test.h"
#include "cmd_run.h"
#include "cmd_clean.h"

static void print_usage(void) {
    Console * c = singleton(Console);
    call(c, print_cstring, "Usage: cdftk <command> [args]\n\n");
    call(c, print_cstring, "Commands:\n");
    call(c, print_cstring, "  create <name>   Create a new CDF project (--type lib|app)\n");
    call(c, print_cstring, "  build         Build the CDF project in current directory\n");
    call(c, print_cstring, "  test          Run tests for the CDF project in current directory\n");
    call(c, print_cstring, "  run           Build and run the CDF project in current directory\n");
    call(c, print_cstring, "  clean         Clean build artifacts\n");
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    String * cmd = new(String, argv[1]);

    if (call(cmd, equals_cstring, "create")) {
        REFCDEC(cmd);
        if (argc < 3) {
            Console * c = singleton(Console);
            call(c, print_cstring, "Usage: cdftk create <project-name> [--type lib|app]\n");
            return 1;
        }
        const char * name = NULL;
        const char * type = "app";
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--type") == 0 && i + 1 < argc) {
                type = argv[++i];
            } else if (!name) {
                name = argv[i];
            }
        }
        if (!name) {
            Console * c = singleton(Console);
            call(c, print_cstring, "Usage: cdftk create <project-name> [--type lib|app]\n");
            return 1;
        }
        return cmd_create_new_project(name, type);
    }

    if (call(cmd, equals_cstring, "build")) {
        REFCDEC(cmd);
        return cmd_build();
    }

    if (call(cmd, equals_cstring, "test")) {
        REFCDEC(cmd);
        return cmd_test();
    }

    if (call(cmd, equals_cstring, "run")) {
        REFCDEC(cmd);
        return cmd_run();
    }

    if (call(cmd, equals_cstring, "clean")) {
        REFCDEC(cmd);
        return cmd_clean();
    }

    REFCDEC(cmd);
    Console * c = singleton(Console);
    call(c, print_cstring, "Unknown command\n");
    return 1;
}
