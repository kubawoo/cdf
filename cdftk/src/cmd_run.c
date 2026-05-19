#include <stdlib.h>
#include <cdf.h>
#include "cmd_run.h"

int cmd_run(void) {
    Console * c = singleton(Console);

    String * path = new(String, "build/Makefile");
    File * f = new(File, path);
    REFCDEC(path);
    if (!call(f, exists)) {
        REFCDEC(f);
        call(c, print_cstring, "Error: build/Makefile not found. Run 'cdftk build' first.\n");
        return 1;
    }
    REFCDEC(f);

    call(c, print_cstring, "Running make -C build run...");
    int ret = system("make -C build run");
    if (ret != 0) {
        call(c, print_cstring, "Run failed\n");
        return 1;
    }

    return 0;
}
