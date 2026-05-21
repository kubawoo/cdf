#include "file.h"
#include "ooc_string.h"
#include "test_framework.h"

void file_new(TEST_CASE_ARGUMENTS) {
    String *path = new(String, "/tmp/test_file_cdf.txt");
    File *file = new(File, path);
    ASSERT_TRUE(type_equal(file, "File"));
    REFCDEC(path);
    REFCDEC(file);
}

void file_open_close(TEST_CASE_ARGUMENTS) {
    String *path = new(String, "/tmp/test_file_cdf_open.txt");
    File *file = new(File, path);
    
    // Test opening file
    bool result = call(file, open, "w");
    ASSERT_TRUE(result);
    
    // Test is_open
    ASSERT_TRUE(call(file, is_open));
    
    // Test closing file
    call(file, close);
    ASSERT_FALSE(call(file, is_open));
    
    // Clean up
    REFCDEC(path);
    REFCDEC(file);
    
    // Remove test file
    system("rm -f /tmp/test_file_cdf_open.txt");
}

void file_exists(TEST_CASE_ARGUMENTS) {
    String *path = new(String, "/tmp/test_file_cdf_exists.txt");
    
    // Test non-existing file
    File *file1 = new(File, path);
    ASSERT_FALSE(call(file1, exists));
    REFCDEC(file1);
    
    // Create file
    system("touch /tmp/test_file_cdf_exists.txt");
    
    // Test existing file
    File *file2 = new(File, path);
    ASSERT_TRUE(call(file2, exists));
    REFCDEC(file2);
    
    // Clean up
    REFCDEC(path);
    system("rm -f /tmp/test_file_cdf_exists.txt");
}

void file_write_read(TEST_CASE_ARGUMENTS) {
    String *path = new(String, "/tmp/test_file_cdf_rw.txt");
    File *file = new(File, path);
    
    // Open for writing
    ASSERT_TRUE(call(file, open, "w"));
    
    // Write data
    String *test_str = new(String, "Hello, CDF!\n");
    ASSERT_TRUE(call(file, write_string, test_str));
    REFCDEC(test_str);
    
    // Flush and close
    ASSERT_TRUE(call(file, flush));
    call(file, close);
    
    // Reopen for reading
    ASSERT_TRUE(call(file, open, "r"));
    
    // Read data
    String *read_str = call(file, read);
    ASSERT_NOT_NULL(read_str);
    ASSERT_STRINGS_EQUAL(call(read_str, to_cstring), "Hello, CDF!\n");
    REFCDEC(read_str);
    
    // Clean up
    call(file, close);
    REFCDEC(path);
    REFCDEC(file);
    system("rm -f /tmp/test_file_cdf_rw.txt");
}

TEST_CASES_BEGIN
    TEST_CASE(file_new);
    TEST_CASE(file_open_close);
    TEST_CASE(file_exists);
    TEST_CASE(file_write_read);
TEST_CASES_END