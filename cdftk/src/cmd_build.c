#define _GNU_SOURCE
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cdf.h>
#include "utils.h"
#include "cmd_build.h"

static void mangle_version(const char * version, char * out, size_t out_size) {
    char buf[128];
    snprintf(buf, sizeof(buf), "%s", version);
    for (char * p = buf; *p; p++)
        if (*p == '-') *p = '.';

    char * parts[4];
    int count = 0;
    parts[count] = strtok(buf, ".");
    while (parts[count] && count < 4) {
        count++;
        parts[count] = strtok(NULL, ".");
    }

    if (count >= 4)
        snprintf(out, out_size, "%s.%s-%s", parts[0], parts[1], parts[3]);
    else
        snprintf(out, out_size, "%s.%s", parts[0], parts[1]);
}

static void dep_lib_name(const char * name, char * out, size_t out_size) {
    char buf[128];
    snprintf(buf, sizeof(buf), "%s", name);
    char * w = buf;
    for (char * r = buf; *r; r++)
        if (*r != '-') *(w++) = *r;
    *w = '\0';
    snprintf(out, out_size, "%s", buf);
}

int cmd_build(void) {
    Console * c = new(Console);
    char cflags[4096] = "";
    char ldflags[4096] = "";
    char test_cflags[4096] = "";
    char test_ldflags[4096] = "";
    char ld_path[4096] = "";
    char test_ld_path[4096] = "";
    char test_runner[512] = "";

    if (access("cdfmodule.json", F_OK) != 0) {
        call(c, print_cstring, "Error: cdfmodule.json not found in current directory\n");
        REFCDEC(c);
        return 1;
    }

    // Read deps: three lines per dep (name, version, scope)
    FILE * fp = popen("jq -r '.dependencies[] | .name, .version, (.scope // \"\")' cdfmodule.json", "r");
    if (!fp) {
        call(c, print_cstring, "Error: could not read cdfmodule.json\n");
        REFCDEC(c);
        return 1;
    }

    char name[128], version[128], scope[128];
    while (fgets(name, sizeof(name), fp) &&
           fgets(version, sizeof(version), fp) &&
           fgets(scope, sizeof(scope), fp)) {
        name[strcspn(name, "\n")] = '\0';
        version[strcspn(version, "\n")] = '\0';
        scope[strcspn(scope, "\n")] = '\0';

        char mangled[64], libname[128], path[512];
        mangle_version(version, mangled, sizeof(mangled));
        snprintf(path, sizeof(path), "$(CDF_HOME)/cdf/%s/%s", name, mangled);
        dep_lib_name(name, libname, sizeof(libname));

        char inc[512], lib[512];
        snprintf(inc, sizeof(inc), "-I%s/include", path);
        snprintf(lib, sizeof(lib), "-L%s/lib -l%s", path, libname);

        if (strcmp(scope, "test") == 0) {
            if (strlen(test_cflags) > 0) strcat(test_cflags, " ");
            strcat(test_cflags, inc);
            if (strlen(test_ldflags) > 0) strcat(test_ldflags, " ");
            strcat(test_ldflags, lib);
            if (strlen(test_ld_path) > 0) strcat(test_ld_path, ":");
            strcat(test_ld_path, path);
            strcat(test_ld_path, "/lib");
            if (strcmp(name, "test-framework") == 0) {
                snprintf(test_runner, sizeof(test_runner),
                    "$(CDF_HOME)/cdf/test-framework/%s/bin/testrunner", mangled);
            }
        } else {
            if (strlen(cflags) > 0) strcat(cflags, " ");
            strcat(cflags, inc);
            if (strlen(ldflags) > 0) strcat(ldflags, " ");
            strcat(ldflags, lib);
            if (strlen(ld_path) > 0) strcat(ld_path, ":");
            strcat(ld_path, path);
            strcat(ld_path, "/lib");
        }
    }
    pclose(fp);

    // Combined LD_LIBRARY_PATH for test runner
    char combined_ld_path[8192] = "";
    if (strlen(ld_path) > 0) strcat(combined_ld_path, ld_path);
    if (strlen(test_ld_path) > 0) {
        if (strlen(combined_ld_path) > 0) strcat(combined_ld_path, ":");
        strcat(combined_ld_path, test_ld_path);
    }

    // Collect all -l flags for test runner
    char testrunner_libs[4096] = "";
    {
        char all_ld[8192];
        snprintf(all_ld, sizeof(all_ld), "%s %s", ldflags, test_ldflags);
        char * token = strtok(all_ld, " ");
        while (token) {
            if (token[0] == '-' && token[1] == 'l') {
                if (strlen(testrunner_libs) > 0) strcat(testrunner_libs, " ");
                strcat(testrunner_libs, token);
            }
            token = strtok(NULL, " ");
        }
    }

    mkdir("build", 0755);

    // Write Makefile.inc
    char makefile_inc[8192];
    snprintf(makefile_inc, sizeof(makefile_inc),
        "CC = gcc -std=c17\n"
        "CFLAGS = -c -Wall -fPIC -Os\n"
        "LDFLAGS =\n"
        "BUILD_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))\n"
        "PROJECT_DIR := $(abspath $(BUILD_DIR)/..)\n"
        "CDF_HOME ?= $(HOME)/.cdf\n"
        "MOD_NAME := $(shell jq -r '.name' $(PROJECT_DIR)/cdfmodule.json)\n"
        "CDF_INCLUDES = %s\n"
        "CDF_LDFLAGS = %s\n"
        "TEST_INCLUDES = %s\n"
        "TEST_LDFLAGS = %s\n"
        "TEST_RUNNER = %s\n",
        cflags, ldflags, test_cflags, test_ldflags,
        strlen(test_runner) > 0 ? test_runner : "echo no-test-runner");

    write_file("build/Makefile.inc", makefile_inc);

    // Write Makefile
    char makefile[8192];
    snprintf(makefile, sizeof(makefile),
        "include Makefile.inc\n"
        "\n"
        "EXECUTABLE = $(MOD_NAME)\n"
        "SOURCES = $(wildcard $(PROJECT_DIR)/src/*.c)\n"
        "OBJECTS = $(SOURCES:$(PROJECT_DIR)/src/%%.c=$(BUILD_DIR)%%.o)\n"
        "TEST_CASES = $(wildcard $(PROJECT_DIR)/test/tc_*.c)\n"
        "TEST_SOS = $(TEST_CASES:$(PROJECT_DIR)/test/%%.c=$(BUILD_DIR)%%.so)\n"
        "\n"
        ".PHONY: all build test clean\n"
        "\n"
        "all: build\n"
        "\n"
        "build: $(BUILD_DIR)$(EXECUTABLE)\n"
        "\n"
        "$(BUILD_DIR)$(EXECUTABLE): $(OBJECTS)\n"
        "\t$(CC) $(OBJECTS) $(CDF_LDFLAGS) -o $@\n"
        "\n"
        "$(BUILD_DIR)%%.o: $(PROJECT_DIR)/src/%%.c\n"
        "\t$(CC) $(CFLAGS) $(CDF_INCLUDES) $< -o $@\n"
        "\n"
        "test: $(TEST_SOS)\n"
        "\tLD_LIBRARY_PATH=\"%s\" $(TEST_RUNNER) %s $(TEST_SOS)\n"
        "\t@echo \"All tests passed!\"\n"
        "\n"
        "$(BUILD_DIR)%%.so: $(PROJECT_DIR)/test/%%.c\n"
        "\t$(CC) -shared -fPIC $(CDF_INCLUDES) $(TEST_INCLUDES) $< -o $@\n"
        "\n"
        "clean:\n"
        "\trm -f $(OBJECTS) $(TEST_SOS) $(BUILD_DIR)$(EXECUTABLE)\n",
        combined_ld_path, testrunner_libs);

    write_file("build/Makefile", makefile);

    printf("Running make -C build...\n");
    int ret = system("make -C build");
    if (ret != 0) {
        call(c, print_cstring, "Build failed\n");
        REFCDEC(c);
        return 1;
    }

    call(c, print_cstring, "Build successful\n");
    REFCDEC(c);
    return 0;
}
