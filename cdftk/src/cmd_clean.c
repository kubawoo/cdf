#include <stdlib.h>
#include <cdf.h>

int cmd_clean(void) {
    Console * c = singleton(Console);
    
    call(c, print_cstring, "Cleaning build directory...\n");
    int ret = system("rm -rf build");
    if (ret != 0) {
        call(c, print_cstring, "Clean failed\n");
        return 1;
    }
    
    call(c, print_cstring, "Clean successful\n");
    return 0;
}