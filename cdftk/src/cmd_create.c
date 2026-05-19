#include <cdf.h>
#include "cmd_create.h"

int cmd_create_new_project(const char * name) {
    Console * c = singleton(Console);
    Directory * dir = new(Directory);
    String * sname = new(String, name);

    if (!call(dir, create, sname)) {
        call(c, print_cstring, "Error: could not create project directory\n");
        REFCDEC(dir);
        REFCDEC(sname);
        return 1;
    }

    String * path = new(String);
    call(path, format, "%s/src", name);
    call(dir, create, path);
    call(path, format, "%s/test", name);
    call(dir, create, path);
    REFCDEC(dir);

    call(path, format, "%s/cdfmodule.json", name);
    String * json = new(String);
    call(json, format,
        "{\n"
        "    \"group\": \"cdf\",\n"
        "    \"name\": \"%s\",\n"
        "    \"version\": \"0.1.0-SNAPSHOT\",\n"
        "    \"description\": \"%s\",\n"
        "    \"dependencies\": [\n"
        "        {\n"
        "            \"group\": \"cdf\",\n"
        "            \"name\": \"cdf\",\n"
        "            \"version\": \"0.4.0\"\n"
        "        },\n"
        "        {\n"
        "            \"group\": \"cdf\",\n"
        "            \"name\": \"test-framework\",\n"
        "            \"version\": \"0.4.0\",\n"
        "            \"scope\": \"test\"\n"
        "        }\n"
        "    ]\n"
        "}\n",
        name, name);
    {
        File * f = new(File, path);
        call(f, open, new(String, "w"));
        call(f, write_string, json);
        REFCDEC(f);
    }
    REFCDEC(json);

    call(path, format, "%s/src/main.c", name);
    String * main_c = new(String);
    call(main_c, format,
        "#include <cdf.h>\n"
        "\n"
        "int main(void) {\n"
        "    String * txt = new(String, \"Hello from %s!\");\n"
        "    Console * c = singleton(Console);\n"
        "\n"
        "    call(c, print_object, txt);\n"
        "\n"
        "    REFCDEC(txt);\n"
        "    return 0;\n"
        "}\n",
        name);
    {
        File * f = new(File, path);
        call(f, open, new(String, "w"));
        call(f, write_string, main_c);
        REFCDEC(f);
    }
    REFCDEC(main_c);

    call(path, format, "%s/test/tc_example.c", name);
    String * test_c = new(String,
        "#include \"test_framework.h\"\n"
        "\n"
        "void test_true(TEST_CASE_ARGUMENTS) {\n"
        "    ASSERT_TRUE(1);\n"
        "}\n"
        "\n"
        "TEST_CASES_BEGIN\n"
        "    TEST_CASE(test_true);\n"
        "TEST_CASES_END\n");
    {
        File * f = new(File, path);
        call(f, open, new(String, "w"));
        call(f, write_string, test_c);
        REFCDEC(f);
    }
    REFCDEC(test_c);

    REFCDEC(path);

    String * msg = new(String);
    call(msg, format, "Project '%s' created successfully", name);
    call(c, print_object, msg);
    REFCDEC(msg);

    REFCDEC(sname);
    return 0;
}
