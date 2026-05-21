#include "directory.h"
#include "ooc_string.h"
#include "test_framework.h"

void directory_new(TEST_CASE_ARGUMENTS) {
    Directory *dir = new(Directory);
    ASSERT_TRUE(type_equal(dir, "Directory"));
    REFCDEC(dir);
}

void directory_create(TEST_CASE_ARGUMENTS) {
    Directory *dir = new(Directory);
    String *path = new(String, "/tmp/test_dir_cdf");
    
    // Test directory creation
    bool result = call(dir, create, path);
    ASSERT_TRUE(result);
    
    // Clean up
    REFCDEC(path);
    REFCDEC(dir);
    
    // Remove test directory
    system("rm -rf /tmp/test_dir_cdf");
}

void directory_create_existing(TEST_CASE_ARGUMENTS) {
    // Create a directory first
    system("mkdir -p /tmp/test_dir_cdf_existing");
    
    Directory *dir = new(Directory);
    String *path = new(String, "/tmp/test_dir_cdf_existing");
    
    // Test creating directory that already exists
    bool result = call(dir, create, path);
    ASSERT_FALSE(result); // Should fail as directory already exists
    
    // Clean up
    REFCDEC(path);
    REFCDEC(dir);
    
    // Remove test directory
    system("rm -rf /tmp/test_dir_cdf_existing");
}

TEST_CASES_BEGIN
    TEST_CASE(directory_new);
    TEST_CASE(directory_create);
    TEST_CASE(directory_create_existing);
TEST_CASES_END