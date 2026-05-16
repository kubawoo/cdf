#include <sys/stat.h>
#include <stdio.h>
#include <cdf.h>
#include "utils.h"
#include "cmd_create.h"

int cmd_create_new_project(const char * name) {
    char buf[1024];
    char content[4096];
    Console * c = new(Console);

    if (mkdir(name, 0755) != 0) {
        call(c, print_cstring, "Error: could not create project directory\n");
        REFCDEC(c);
        return 1;
    }

    snprintf(buf, sizeof(buf), "%s/src", name);
    mkdir(buf, 0755);

    snprintf(buf, sizeof(buf), "%s/test", name);
    mkdir(buf, 0755);

    snprintf(buf, sizeof(buf), "%s/cdfmodule.json", name);
    snprintf(content, sizeof(content),
        "{\n"
        "    \"group\": \"cdf\",\n"
        "    \"name\": \"%s\",\n"
        "    \"version\": \"0.1.0-SNAPSHOT\",\n"
        "    \"description\": \"%s\",\n"
        "    \"dependencies\": [\n"
        "        {\n"
        "            \"group\": \"cdf\",\n"
        "            \"name\": \"cdf-core\",\n"
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
    write_file(buf, content);

    snprintf(buf, sizeof(buf), "%s/src/main.c", name);
    snprintf(content, sizeof(content),
        "#include <cdf.h>\n"
        "#include <stdio.h>\n"
        "\n"
        "int main(void) {\n"
        "    String * txt = new(String, \"Hello from %s!\");\n"
        "    Console * c = new(Console);\n"
        "\n"
        "    call(c, print_object, txt);\n"
        "\n"
        "    REFCDEC(c);\n"
        "    REFCDEC(txt);\n"
        "    return 0;\n"
        "}\n",
        name);
    write_file(buf, content);

    snprintf(buf, sizeof(buf), "%s/test/tc_example.c", name);
    write_file(buf,
        "#include \"test_framework.h\"\n"
        "\n"
        "void test_true(TEST_CASE_ARGUMENTS) {\n"
        "    ASSERT_TRUE(1);\n"
        "}\n"
        "\n"
        "TEST_CASES_BEGIN\n"
        "    TEST_CASE(test_true);\n"
        "TEST_CASES_END\n");

    printf("Project '%s' created successfully\n", name);
    REFCDEC(c);
    return 0;
}
