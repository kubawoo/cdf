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
    system("rm -rf /tmp/test_dir_cdf");
}

static void directory_create_existing(void)
{
    // Create a directory first
    system("mkdir -p /tmp/test_dir_cdf_existing");
    
    Directory *dir = new(Directory);
    String *path = new(String, "/tmp/test_dir_cdf_existing");
    
    // Test creating directory that already exists
    bool result = call(dir, create, path);
    assert(!(result)); // Should fail as directory already exists
    
    // Clean up
    REFCDEC(path);
    REFCDEC(dir);
    
    // Remove test directory
    system("rm -rf /tmp/test_dir_cdf_existing");
}
int main(void)
{
    directory_new();
    directory_create();
    directory_create_existing();
    return 0;
}
