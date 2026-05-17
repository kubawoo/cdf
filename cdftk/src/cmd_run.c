#include <stdlib.h>
#include <cdf.h>
#include "cmd_run.h"

int cmd_run(void) {
    Console * c = singleton(Console);

    File * f = new(File, new(String, "build/Makefile"));
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
