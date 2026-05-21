#include "fileinputstream.h"
#include "test_framework.h"


void fileinputstream_test(TEST_CASE_ARGUMENTS) {
    String * filename = new(String, "test.txt");
    File * file = new(File, filename);
    REFCDEC(filename);
    InputStream * is = new(FileInputStream, file);

    String * expected = new(String, "test 123\nfoo bar baz\n");

    for(int i = 0; i < expected->length; i++) {
        int c = call(is, read);
        ASSERT_TRUE(c == call(expected, char_at, i));
    }

    REFCDEC(is);
    REFCDEC(file);
    REFCDEC(expected);
}

void file_not_found(TEST_CASE_ARGUMENTS) {
    String * filename = new(String, "no_such_file.txt");
    File * file = new(File, filename);
    REFCDEC(filename);

    InputStream * is = new(FileInputStream, file);
    int c = call(is, read);
    ASSERT_TRUE(c == -1);

    REFCDEC(is);
    REFCDEC(file);
}

TEST_CASES_BEGIN
    TEST_CASE(fileinputstream_test);
    TEST_CASE(file_not_found);
TEST_CASES_END

