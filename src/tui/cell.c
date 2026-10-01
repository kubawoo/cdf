#include "cell.h"
#include <locale.h>

bool cell_equal(const Cell * a, const Cell * b) {
    return a->ch == b->ch
        && a->fg == b->fg
        && a->bg == b->bg
        && a->attrs == b->attrs
        && a->wide_lead == b->wide_lead
        && a->wide_tail == b->wide_tail;
}

/* Candidates in order of preference. C.UTF-8 is the portable spelling; the
   others cover glibc and musl naming. */
static const char * const utf8_locales[] = {
    "C.UTF-8",
    "en_US.UTF-8",
    "en_GB.UTF-8",
    "C.utf8",
    "UTF-8",
    NULL
};

bool tui_init_utf8(void) {
    for(int i = 0; utf8_locales[i] != NULL; ++i) {
        if(setlocale(LC_CTYPE, utf8_locales[i]) != NULL) {
            return true;
        }
    }
    /* No UTF-8 locale is installed. ASCII still lays out correctly; only
       wide-character widths stay approximate. */
    return setlocale(LC_CTYPE, NULL) != NULL;
}
