#include "fileinputstream.h"
#include "fileoutputstream.h"
#include "test_framework.h"

void fileoutputstream_test(TEST_CASE_ARGUMENTS) {
    String * filename = new(String, "fileoutputstream_test.log");
    File * file = new(File, filename);
    REFCDEC(filename);

    OutputStream * os = new(FileOutputStream, file);
    call(os, write, 'a');
    call(os, write, 'b');
    call(os, write, 'c');
    call(os, write, 'd');

    String * s = new(String, "efgh");
    call(os, write_string, s);
    REFCDEC(s);
    REFCDEC(os);

    String * expected = new(String, "abcdefgh");
    InputStream * is = new(FileInputStream, file);

    for(int i = 0; i < expected->length; i++) {
        int c = call(is, read);
        ASSERT_TRUE(c == call(expected, char_at, i));
    }

    REFCDEC(expected);
    REFCDEC(is);
    REFCDEC(file);
}

void fileoutputstream_append_test(TEST_CASE_ARGUMENTS) {
    String * filename = new(String, "fileoutputstream_test.log");
    File * file = new(File, filename);
    REFCDEC(filename);

    OutputStream * os = new(FileOutputStream, file, false);
    call(os, write, 'a');
    call(os, write, 'b');
    call(os, write, 'c');
    call(os, write, 'd');
    REFCDEC(os);

    os = new(FileOutputStream, file, true);
    String * s = new(String, "foobar\n");
    call(os, write_string, s);
    REFCDEC(s);
    REFCDEC(os);

    String * expected = new(String, "abcdfoobar\n");
    InputStream * is = new(FileInputStream, file);

    for(int i = 0; i < expected->length; i++) {
        int c = call(is, read);
        ASSERT_TRUE(c == call(expected, char_at, i));
    }

    REFCDEC(expected);
    REFCDEC(is);
    REFCDEC(file);
}

TEST_CASES_BEGIN
    TEST_CASE(fileoutputstream_test);
    TEST_CASE(fileoutputstream_append_test);
TEST_CASES_END

