#include <stdlib.h>
#include <cdf.h>
#include <json.h>
#include "cmd_build.h"

static String * mangle_version(String * version) {
    String * s = new(String);
    for (int i = 0; i < version->length; i++) {
        char c = call(version, char_at, i);
        call(s, append_char, c == '-' ? '.' : c);
    }
    StringTokenizer * st = new(StringTokenizer, s);
    List * parts = call(st, split_by_char, '.');
    REFCDEC(st);
    REFCDEC(s);

    String * p0 = call(parts, get, 0);
    String * p1 = call(parts, get, 1);
    String * result = new(String);
    call(result, append, p0);
    call(result, append_char, '.');
    call(result, append, p1);
    if (parts->length >= 4) {
        String * p3 = call(parts, get, 3);
        call(result, append_char, '-');
        call(result, append, p3);
        REFCDEC(p3);
    }
    REFCDEC(p1);
    REFCDEC(p0);
    REFCDEC(parts);
    return result;
}

static String * dep_lib_name(String * name) {
    String * result = new(String);
    for (int i = 0; i < name->length; i++) {
        char c = call(name, char_at, i);
        if (c != '-')
            call(result, append_char, c);
    }
    return result;
}

int cmd_build(void) {
    Console * c = singleton(Console);

    File * f = new(File, new(String, "cdfmodule.json"));
    if (!call(f, exists)) {
        REFCDEC(f);
        call(c, print_cstring, "Error: cdfmodule.json not found in current directory\n");
        return 1;
    }
    REFCDEC(f);

    f = new(File, new(String, "cdfmodule.json"));
    call(f, open, new(String, "r"));
    String * content = call(f, read);
    call(f, close);
    REFCDEC(f);

    StringInputStream * sis = new(StringInputStream, content);
    JsonObjectBuilderEventsHandler * handler = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * parser = new(JsonEventsParser, (JsonEventsHandler *)handler);
    int parse_result = call(parser, parse, (InputStream *)sis);
    REFCDEC(sis);
    REFCDEC(parser);

    if (parse_result != CJSON_PARSE_SUCCESS) {
        REFCDEC(handler);
        REFCDEC(content);
        call(c, print_cstring, "Error: could not parse cdfmodule.json\n");
        return 1;
    }

    JsonObject * root = handler->object;
    REFCINC(root);

    String * cflags = new(String);
    String * ldflags = new(String);
    String * test_cflags = new(String);
    String * test_ldflags = new(String);
    String * ld_path = new(String);
    String * test_ld_path = new(String);
    String * test_runner = new(String);

    String * deps_key = new(String, "dependencies");
    Object * deps_obj = call(root, get_value, deps_key);
    REFCDEC(deps_key);

    if (deps_obj && type_equal(deps_obj, "List")) {
        List * deps = (List *)deps_obj;
        String * path = new(String);

        for (int i = 0; i < deps->length; i++) {
            JsonObject * dep = call(deps, get, i);

            String * name_key = new(String, "name");
            String * dep_name = call(dep, get_value, name_key);
            REFCDEC(name_key);

            String * ver_key = new(String, "version");
            String * dep_version = call(dep, get_value, ver_key);
            REFCDEC(ver_key);

            String * scope_key = new(String, "scope");
            Object * scope_obj = call(dep, get_value, scope_key);
            REFCDEC(scope_key);
            String * dep_scope = NULL;
            if (scope_obj) {
                dep_scope = call(scope_obj, to_string);
                REFCDEC(scope_obj);
            }

            String * mangled = mangle_version(dep_version);
            String * libname = dep_lib_name(dep_name);

            call(path, format, "$(CDF_HOME)/cdf/%s/%s", call(dep_name, to_cstring), call(mangled, to_cstring));

            String * inc = new(String);
            call(inc, format, "-I%s/include", call(path, to_cstring));
            String * lib = new(String);
            call(lib, format, "-L%s/lib -l%s", call(path, to_cstring), call(libname, to_cstring));

            bool is_test = dep_scope && call(dep_scope, equals_cstring, "test");
            String * target_cflags = is_test ? test_cflags : cflags;
            String * target_ldflags = is_test ? test_ldflags : ldflags;
            String * target_path = is_test ? test_ld_path : ld_path;

            if (target_cflags->length > 0) call(target_cflags, append_char, ' ');
            call(target_cflags, append, inc);

            if (target_ldflags->length > 0) call(target_ldflags, append_char, ' ');
            call(target_ldflags, append, lib);

            if (target_path->length > 0) call(target_path, append_char, ':');
            call(target_path, append, path);
            call(target_path, append_cstring, "/lib");

            if (is_test && dep_scope && call(dep_name, equals_cstring, "test-framework")) {
                call(test_runner, format, "$(CDF_HOME)/cdf/test-framework/%s/bin/testrunner", call(mangled, to_cstring));
            }

            if (dep_scope) REFCDEC(dep_scope);
            REFCDEC(libname);
            REFCDEC(mangled);
            REFCDEC(lib);
            REFCDEC(inc);
            REFCDEC(dep_version);
            REFCDEC(dep_name);
            REFCDEC(dep);
        }
        REFCDEC(path);
    }
    if (deps_obj) REFCDEC(deps_obj);

    String * combined_ld_path = new(String);
    if (ld_path->length > 0) call(combined_ld_path, append, ld_path);
    if (test_ld_path->length > 0) {
        if (combined_ld_path->length > 0) call(combined_ld_path, append_char, ':');
        call(combined_ld_path, append, test_ld_path);
    }

    String * testrunner_libs = new(String);
    {
        StringTokenizer * tokenizer = new(StringTokenizer, ldflags);
        List * tokens = call(tokenizer, split_by_char, ' ');
        REFCDEC(tokenizer);
        for (int i = 0; i < tokens->length; i++) {
            String * token = call(tokens, get, i);
            if (call(token, char_at, 0) == '-' && call(token, char_at, 1) == 'l') {
                if (testrunner_libs->length > 0) call(testrunner_libs, append_char, ' ');
                call(testrunner_libs, append, token);
            }
            REFCDEC(token);
        }
        REFCDEC(tokens);

        tokenizer = new(StringTokenizer, test_ldflags);
        tokens = call(tokenizer, split_by_char, ' ');
        REFCDEC(tokenizer);
        for (int i = 0; i < tokens->length; i++) {
            String * token = call(tokens, get, i);
            if (call(token, char_at, 0) == '-' && call(token, char_at, 1) == 'l') {
                if (testrunner_libs->length > 0) call(testrunner_libs, append_char, ' ');
                call(testrunner_libs, append, token);
            }
            REFCDEC(token);
        }
        REFCDEC(tokens);
    }

    Directory * dir = new(Directory);
    call(dir, create, new(String, "build"));
    REFCDEC(dir);

    String * makefile_inc = new(String);
    call(makefile_inc, format,
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
        call(cflags, to_cstring), call(ldflags, to_cstring),
        call(test_cflags, to_cstring), call(test_ldflags, to_cstring),
        test_runner->length > 0 ? call(test_runner, to_cstring) : "echo no-test-runner");

    f = new(File, new(String, "build/Makefile.inc"));
    call(f, open, new(String, "w"));
    call(f, write_string, makefile_inc);
    REFCDEC(f);
    REFCDEC(makefile_inc);

    String * makefile = new(String);
    call(makefile, format,
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
        "run: build\n"
        "\tLD_LIBRARY_PATH=\"%s\" $(BUILD_DIR)$(EXECUTABLE)\n"
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
        call(ld_path, to_cstring),
        call(combined_ld_path, to_cstring), call(testrunner_libs, to_cstring));

    f = new(File, new(String, "build/Makefile"));
    call(f, open, new(String, "w"));
    call(f, write_string, makefile);
    REFCDEC(f);
    REFCDEC(makefile);

    REFCDEC(testrunner_libs);
    REFCDEC(combined_ld_path);
    REFCDEC(test_runner);
    REFCDEC(test_ld_path);
    REFCDEC(ld_path);
    REFCDEC(test_ldflags);
    REFCDEC(test_cflags);
    REFCDEC(ldflags);
    REFCDEC(cflags);

    REFCDEC(root);
    REFCDEC(handler);
    REFCDEC(content);

    call(c, print_cstring, "Running make -C build...");
    int ret = system("make -C build");
    if (ret != 0) {
        call(c, print_cstring, "Build failed\n");
        return 1;
    }

    call(c, print_cstring, "Build successful\n");
    return 0;
}