#include <stdlib.h>
#include <cdf.h>
#include "cmd_test.h"

int cmd_test(void) {
    Console * c = singleton(Console);

    File * f = new(File, new(String, "cdfmodule.json"));
    if (!call(f, exists)) {
        REFCDEC(f);
        call(c, print_cstring, "Error: cdfmodule.json not found in current directory\n");
        return 1;
    }
    REFCDEC(f);

    f = new(File, new(String, "build/Makefile"));
    if (!call(f, exists)) {
        REFCDEC(f);
        call(c, print_cstring, "Error: build/Makefile not found. Run 'cdftk build' first.\n");
        return 1;
    }
    REFCDEC(f);

    call(c, print_cstring, "Running make -C build test...");
    int ret = system("make -C build test");
    if (ret != 0) {
        call(c, print_cstring, "Tests failed\n");
        return 1;
    }

    call(c, print_cstring, "All tests passed!\n");
    return 0;
}
