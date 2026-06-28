#include "file.h"
#include "ooc_string.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void file_new(void)
{
    String *path = new(String, "/tmp/test_file_cdf.txt");
    File *file = new(File, path);
    assert(type_equal(file, "File"));
    REFCDEC(path);
    REFCDEC(file);
}

static void file_open_close(void)
{
    String *path = new(String, "/tmp/test_file_cdf_open.txt");
    File *file = new(File, path);
    
    // Test opening file
    bool result = call(file, open, "w");
    assert(result);
    
    // Test is_open
    assert(call(file, is_open));
    
    // Test closing file
    call(file, close);
    assert(!(call(file, is_open)));
    
    // Clean up
    REFCDEC(path);
    REFCDEC(file);
    
    // Remove test file
    system("rm -f /tmp/test_file_cdf_open.txt");
}

static void file_exists(void)
{
    String *path = new(String, "/tmp/test_file_cdf_exists.txt");
    
    // Test non-existing file
    File *file1 = new(File, path);
    assert(!(call(file1, exists)));
    REFCDEC(file1);
    
    // Create file
    system("touch /tmp/test_file_cdf_exists.txt");
    
    // Test existing file
    File *file2 = new(File, path);
    assert(call(file2, exists));
    REFCDEC(file2);
    
    // Clean up
    REFCDEC(path);
    system("rm -f /tmp/test_file_cdf_exists.txt");
}

static void file_write_read(void)
{
    String *path = new(String, "/tmp/test_file_cdf_rw.txt");
    File *file = new(File, path);
    
    // Open for writing
    assert(call(file, open, "w"));
    
    // Write data
    String *test_str = new(String, "Hello, CDF!\n");
    assert(call(file, write_string, test_str));
    REFCDEC(test_str);
    
    // Flush and close
    assert(call(file, flush));
    call(file, close);
    
    // Reopen for reading
    assert(call(file, open, "r"));
    
    // Read data
    String *read_str = call(file, read);
    assert(read_str != NULL);
    assert(strcmp(call(read_str, to_cstring), "Hello, CDF!\n") == 0);
    REFCDEC(read_str);
    
    // Clean up
    call(file, close);
    REFCDEC(path);
    REFCDEC(file);
    system("rm -f /tmp/test_file_cdf_rw.txt");
}
int main(void)
{
    file_new();
    file_open_close();
    file_exists();
    file_write_read();
    return 0;
}
