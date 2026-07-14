#include "fileinputstream.h"
#include <assert.h>


static void fileinputstream_test(void)
{
    String * filename = new(String, "test.txt");
    File * file = new(File, filename);
    REFCDEC(filename);
    InputStream * is = new(FileInputStream, file);

    String * expected = new(String, "test 123\nfoo bar baz\n");

    for(int i = 0; i < expected->length; i++) {
        int c = call(is, read);
        assert(c == call(expected, char_at, i));
    }

    REFCDEC(is);
    REFCDEC(file);
    REFCDEC(expected);
}

static void file_not_found(void)
{
    String * filename = new(String, "no_such_file.txt");
    File * file = new(File, filename);
    REFCDEC(filename);

    InputStream * is = new(FileInputStream, file);
    int c = call(is, read);
    assert(c == -1);

    REFCDEC(is);
    REFCDEC(file);
}
int main(void)
{
    fileinputstream_test();
    file_not_found();
    return 0;
}


