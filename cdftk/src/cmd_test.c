#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cdf.h>
#include "cmd_test.h"

int cmd_test(void) {
    Console * c = new(Console);

    if (access("cdfmodule.json", F_OK) != 0) {
        call(c, print_cstring, "Error: cdfmodule.json not found in current directory\n");
        REFCDEC(c);
        return 1;
    }

    if (access("build/Makefile", F_OK) != 0) {
        call(c, print_cstring, "Error: build/Makefile not found. Run 'cdftk build' first.\n");
        REFCDEC(c);
        return 1;
    }

    printf("Running make -C build test...\n");
    int ret = system("make -C build test");
    if (ret != 0) {
        call(c, print_cstring, "Tests failed\n");
        REFCDEC(c);
        return 1;
    }

    call(c, print_cstring, "All tests passed!\n");
    REFCDEC(c);
    return 0;
}
