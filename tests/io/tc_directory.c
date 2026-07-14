#include "directory.h"
#include <assert.h>


static void directory_new(void)
{
    Directory *dir = new(Directory);
    assert(type_equal(dir, "Directory"));
    REFCDEC(dir);
}

static void directory_create(void)
{
    Directory *dir = new(Directory);
    String *path = new(String, "/tmp/test_dir_cdf");
    
    // Test directory creation
    bool result = call(dir, create, path);
    assert(result);
    
    // Clean up
    REFCDEC(path);
    REFCDEC(dir);
    
    // Remove test directory
    int ok = system("rm -rf /tmp/test_dir_cdf");
    assert(ok == 0);
}

static void directory_create_existing(void)
{
    // Create a directory first
    int ok = system("mkdir -p /tmp/test_dir_cdf_existing");
    assert(ok == 0);
    
    Directory *dir = new(Directory);
    String *path = new(String, "/tmp/test_dir_cdf_existing");
    
    // Test creating directory that already exists
    bool result = call(dir, create, path);
    assert(!(result)); // Should fail as directory already exists
    
    // Clean up
    REFCDEC(path);
    REFCDEC(dir);
    
    // Remove test directory
    ok = system("rm -rf /tmp/test_dir_cdf_existing");
    assert(ok == 0);
}
int main(void)
{
    directory_new();
    directory_create();
    directory_create_existing();
    return 0;
}
