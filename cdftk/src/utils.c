#include <stdio.h>
#include "utils.h"

int write_file(const char * path, const char * content) {
    FILE * f = fopen(path, "w");
    if (!f) return -1;
    int ret = fprintf(f, "%s", content);
    fclose(f);
    return ret < 0 ? -1 : 0;
}
