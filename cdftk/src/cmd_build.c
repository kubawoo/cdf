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

static int version_compare(String * v1, String * v2) {
    String * s1 = new(String);
    for (int i = 0; i < v1->length; i++) {
        char c = call(v1, char_at, i);
        call(s1, append_char, c == '-' ? '.' : c);
    }
    String * s2 = new(String);
    for (int i = 0; i < v2->length; i++) {
        char c = call(v2, char_at, i);
        call(s2, append_char, c == '-' ? '.' : c);
    }

    StringTokenizer * st1 = new(StringTokenizer, s1);
    List * parts1 = call(st1, split_by_char, '.');
    REFCDEC(st1);
    REFCDEC(s1);

    StringTokenizer * st2 = new(StringTokenizer, s2);
    List * parts2 = call(st2, split_by_char, '.');
    REFCDEC(st2);
    REFCDEC(s2);

    int result = 0;
    int max_len = parts1->length > parts2->length ? parts1->length : parts2->length;
    for (int i = 0; i < max_len && result == 0; i++) {
        if (i >= parts2->length) {
            result = -1;
            break;
        }
        if (i >= parts1->length) {
            result = 1;
            break;
        }

        String * p1 = call(parts1, get, i);
        String * p2 = call(parts2, get, i);

        char * end1, * end2;
        long n1 = strtol(call(p1, to_cstring), &end1, 10);
        long n2 = strtol(call(p2, to_cstring), &end2, 10);

        if (*end1 == '\0' && *end2 == '\0') {
            if (n1 < n2) result = -1;
            else if (n1 > n2) result = 1;
        } else if (*end1 == '\0') {
            result = 1;
        } else if (*end2 == '\0') {
            result = -1;
        } else {
            int cmp = strcmp(call(p1, to_cstring), call(p2, to_cstring));
            if (cmp < 0) result = -1;
            else if (cmp > 0) result = 1;
        }

        REFCDEC(p1);
        REFCDEC(p2);
    }

    REFCDEC(parts1);
    REFCDEC(parts2);
    return result;
}

static JsonObject * read_cdfmodule(const char * cdf_home, String * name, String * mangled) {
    String * json_path = new(String);
    call(json_path, format, "%s/cdf/%s/%s/cdfmodule.json", cdf_home, call(name, to_cstring), call(mangled, to_cstring));

    File * f = new(File, json_path);
    REFCDEC(json_path);
    if (!call(f, exists)) {
        REFCDEC(f);
        return NULL;
    }
    call(f, open, new(String, "r"));
    String * content = call(f, read);
    call(f, close);
    REFCDEC(f);
    if (!content || content->length == 0) {
        if (content) REFCDEC(content);
        return NULL;
    }

    StringInputStream * sis = new(StringInputStream, content);
    JsonObjectBuilderEventsHandler * h = new(JsonObjectBuilderEventsHandler);
    JsonEventsParser * p = new(JsonEventsParser, (JsonEventsHandler *)h);
    int parse_result = call(p, parse, (InputStream *)sis);
    REFCDEC(sis);
    REFCDEC(p);
    REFCDEC(content);

    if (parse_result != CJSON_PARSE_SUCCESS) {
        REFCDEC(h);
        return NULL;
    }

    JsonObject * root = h->object;
    REFCINC(root);
    REFCDEC(h);
    return root;
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
    REFCDEC(handler);

    const char * env = getenv("CDF_HOME");
    String * cdf_home_str;
    if (env) {
        cdf_home_str = new(String, env);
    } else {
        const char * home = getenv("HOME");
        if (home) {
            cdf_home_str = new(String);
            call(cdf_home_str, format, "%s/.cdf", home);
        } else {
            cdf_home_str = new(String, "/home/.cdf");
        }
    }
    const char * cdf_home = call(cdf_home_str, to_cstring);

    Map * resolved = new(Map);
    List * main_names = new(List);
    Map * processed = new(Map);

    List * q_names = new(List);
    List * q_versions = new(List);
    List * q_is_test = new(List);

    String * deps_key = new(String, "dependencies");
    Object * deps_obj = call(root, get_value, deps_key);
    REFCDEC(deps_key);

    if (deps_obj && type_equal(deps_obj, "List")) {
        List * deps = (List *)deps_obj;
        for (int i = 0; i < deps->length; i++) {
            JsonObject * dep = call(deps, get, i);

            String * nk = new(String, "name");
            String * dep_name = call(dep, get_value, nk);
            REFCDEC(nk);

            String * vk = new(String, "version");
            String * dep_version = call(dep, get_value, vk);
            REFCDEC(vk);

            String * sk = new(String, "scope");
            Object * scope_obj = call(dep, get_value, sk);
            REFCDEC(sk);
            bool is_test = false;
            if (scope_obj) {
                String * ss = call(scope_obj, to_string);
                is_test = call(ss, equals_cstring, "test");
                REFCDEC(ss);
                REFCDEC(scope_obj);
            }

            Object * existing = call(resolved, get, dep_name);
            bool should_enqueue = false;
            if (!existing) {
                call(resolved, put, dep_name, dep_version);
                should_enqueue = true;
            } else {
                if (version_compare(dep_version, (String *)existing) > 0) {
                    call(resolved, put, dep_name, dep_version);
                    should_enqueue = true;
                }
                REFCDEC(existing);
            }

            if (should_enqueue) {
                call(q_names, add, dep_name);
                call(q_versions, add, dep_version);
                Boolean * tf = new(Boolean, is_test);
                call(q_is_test, add, tf);
                REFCDEC(tf);
            }

            if (!is_test && !call(main_names, contains, dep_name)) {
                call(main_names, add, dep_name);
            }

            REFCDEC(dep_version);
            REFCDEC(dep_name);
            REFCDEC(dep);
        }
    }
    if (deps_obj) REFCDEC(deps_obj);

    while (q_names->length > 0) {
        String * name = call(q_names, get, 0);
        call(q_names, remove, 0);
        String * version = call(q_versions, get, 0);
        call(q_versions, remove, 0);
        Boolean * is_test_flag = call(q_is_test, get, 0);
        call(q_is_test, remove, 0);
        bool is_test = is_test_flag->value;
        REFCDEC(is_test_flag);

        Object * proc_ver = call(processed, get, name);
        if (proc_ver) {
            if (call(proc_ver, equals, version)) {
                REFCDEC(proc_ver);
                REFCDEC(version);
                REFCDEC(name);
                continue;
            }
            REFCDEC(proc_ver);
        }
        call(processed, put, name, version);

        String * mangled = mangle_version(version);
        JsonObject * dep_root = read_cdfmodule(cdf_home, name, mangled);
        REFCDEC(mangled);

        if (!dep_root) {
            REFCDEC(version);
            REFCDEC(name);
            continue;
        }

        String * dk = new(String, "dependencies");
        Object * dep_deps_obj = call(dep_root, get_value, dk);
        REFCDEC(dk);

        if (dep_deps_obj && type_equal(dep_deps_obj, "List")) {
            List * dep_deps = (List *)dep_deps_obj;
            for (int j = 0; j < dep_deps->length; j++) {
                JsonObject * sub_dep = call(dep_deps, get, j);

                String * snk = new(String, "name");
                String * sub_name = call(sub_dep, get_value, snk);
                REFCDEC(snk);

                String * svk = new(String, "version");
                String * sub_version = call(sub_dep, get_value, svk);
                REFCDEC(svk);

                String * ssk = new(String, "scope");
                Object * sub_scope_obj = call(sub_dep, get_value, ssk);
                REFCDEC(ssk);
                bool sub_edge_test = false;
                if (sub_scope_obj) {
                    String * ss = call(sub_scope_obj, to_string);
                    sub_edge_test = call(ss, equals_cstring, "test");
                    REFCDEC(ss);
                    REFCDEC(sub_scope_obj);
                }

                bool sub_is_test = is_test || sub_edge_test;

                Object * existing = call(resolved, get, sub_name);
                bool should_enqueue = false;
                if (!existing) {
                    call(resolved, put, sub_name, sub_version);
                    should_enqueue = true;
                } else {
                    if (version_compare(sub_version, (String *)existing) > 0) {
                        call(resolved, put, sub_name, sub_version);
                        should_enqueue = true;
                    }
                    REFCDEC(existing);
                }

                if (should_enqueue) {
                    call(q_names, add, sub_name);
                    call(q_versions, add, sub_version);
                    Boolean * tf = new(Boolean, sub_is_test);
                    call(q_is_test, add, tf);
                    REFCDEC(tf);
                }

                if (!sub_is_test && !call(main_names, contains, sub_name)) {
                    call(main_names, add, sub_name);
                }

                REFCDEC(sub_version);
                REFCDEC(sub_name);
                REFCDEC(sub_dep);
            }
        }
        if (dep_deps_obj) REFCDEC(dep_deps_obj);

        REFCDEC(dep_root);
        REFCDEC(version);
        REFCDEC(name);
    }

    String * cflags = new(String);
    String * ldflags = new(String);
    String * test_cflags = new(String);
    String * test_ldflags = new(String);
    String * ld_path = new(String);
    String * test_ld_path = new(String);
    String * test_runner = new(String);

    for (int i = 0; i < main_names->length; i++) {
        String * dep_name = call(main_names, get, i);
        String * dep_version = call(resolved, get, dep_name);

        String * mangled = mangle_version(dep_version);
        String * libname = dep_lib_name(dep_name);
        String * path_prefix = new(String);
        call(path_prefix, format, "$(CDF_HOME)/cdf/%s/%s", call(dep_name, to_cstring), call(mangled, to_cstring));

        String * inc = new(String);
        call(inc, format, "-I%s/include", call(path_prefix, to_cstring));
        String * lib = new(String);
        call(lib, format, "-L%s/lib -l%s", call(path_prefix, to_cstring), call(libname, to_cstring));

        if (cflags->length > 0) call(cflags, append_char, ' ');
        call(cflags, append, inc);
        if (ldflags->length > 0) call(ldflags, append_char, ' ');
        call(ldflags, append, lib);
        if (ld_path->length > 0) call(ld_path, append_char, ':');
        call(ld_path, append, path_prefix);
        call(ld_path, append_cstring, "/lib");

        REFCDEC(path_prefix);
        REFCDEC(libname);
        REFCDEC(mangled);
        REFCDEC(lib);
        REFCDEC(inc);
        REFCDEC(dep_version);
        REFCDEC(dep_name);
    }

    List * all_keys = call(resolved, get_keys);
    for (int i = 0; i < all_keys->length; i++) {
        String * dep_name = call(all_keys, get, i);
        String * dep_version = call(resolved, get, dep_name);

        String * mangled = mangle_version(dep_version);
        String * libname = dep_lib_name(dep_name);
        String * path_prefix = new(String);
        call(path_prefix, format, "$(CDF_HOME)/cdf/%s/%s", call(dep_name, to_cstring), call(mangled, to_cstring));

        String * inc = new(String);
        call(inc, format, "-I%s/include", call(path_prefix, to_cstring));
        String * lib = new(String);
        call(lib, format, "-L%s/lib -l%s", call(path_prefix, to_cstring), call(libname, to_cstring));

        if (test_cflags->length > 0) call(test_cflags, append_char, ' ');
        call(test_cflags, append, inc);
        if (test_ldflags->length > 0) call(test_ldflags, append_char, ' ');
        call(test_ldflags, append, lib);
        if (test_ld_path->length > 0) call(test_ld_path, append_char, ':');
        call(test_ld_path, append, path_prefix);
        call(test_ld_path, append_cstring, "/lib");

        if (call(dep_name, equals_cstring, "test-framework")) {
            call(test_runner, format, "$(CDF_HOME)/cdf/test-framework/%s/bin/testrunner", call(mangled, to_cstring));
        }

        REFCDEC(path_prefix);
        REFCDEC(libname);
        REFCDEC(mangled);
        REFCDEC(lib);
        REFCDEC(inc);
        REFCDEC(dep_version);
        REFCDEC(dep_name);
    }
    REFCDEC(all_keys);

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
        "MOD_TYPE := $(shell jq -r '.type // \"lib\"' $(PROJECT_DIR)/cdfmodule.json)\n"
        "MOD_LIB_NAME := $(shell echo $(MOD_NAME) | sed 's/-//g')\n"
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

    String * run_target = new(String);
    call(run_target, format,
        "ifneq ($(filter app,$(MOD_TYPE)),)\n"
        "run: build\n"
        "\tLD_LIBRARY_PATH=\"%s\" $(BUILD_DIR)$(EXECUTABLE)\n"
        "endif\n",
        call(ld_path, to_cstring));

    String * makefile = new(String);
    call(makefile, format,
        "include Makefile.inc\n"
        "\n"
        "EXECUTABLE = $(if $(filter app,$(MOD_TYPE)),$(MOD_NAME),lib$(MOD_LIB_NAME).so)\n"
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
        "\t$(CC) $(OBJECTS) $(if $(filter app,$(MOD_TYPE)),$(CDF_LDFLAGS),-shared $(CDF_LDFLAGS)) -o $@\n"
        "\n"
        "$(BUILD_DIR)%%.o: $(PROJECT_DIR)/src/%%.c\n"
        "\t$(CC) $(CFLAGS) $(CDF_INCLUDES) $< -o $@\n"
        "\n"
        "%s"
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
        call(run_target, to_cstring),
        call(combined_ld_path, to_cstring), call(testrunner_libs, to_cstring));

    f = new(File, new(String, "build/Makefile"));
    call(f, open, new(String, "w"));
    call(f, write_string, makefile);
    REFCDEC(f);
    REFCDEC(makefile);
    REFCDEC(run_target);

    REFCDEC(testrunner_libs);
    REFCDEC(combined_ld_path);
    REFCDEC(test_runner);
    REFCDEC(test_ld_path);
    REFCDEC(ld_path);
    REFCDEC(test_ldflags);
    REFCDEC(test_cflags);
    REFCDEC(ldflags);
    REFCDEC(cflags);

    REFCDEC(q_names);
    REFCDEC(q_versions);
    REFCDEC(q_is_test);
    REFCDEC(processed);
    REFCDEC(main_names);
    REFCDEC(resolved);
    REFCDEC(cdf_home_str);
    REFCDEC(root);
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
