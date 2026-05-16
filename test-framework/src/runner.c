#include "test_framework.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int libs_count = 0;
static void ** libs = NULL;

static void closeLibs() {
	if(libs != NULL) {
	    for(int i = libs_count - 1; i >= 0; --i) {
	        int ret = dlclose(libs[i]);
	        if(ret) {
	            fprintf(stderr, "Failed to close lib %d\n", i);
	        }
	    }
	    free(libs);
	}
}

void cleanSuite(TestSuite * suite) {
    for(int i = 0; i < suite->count; ++i) {
        if(suite->cases[i]->result->msg != NULL) {
            free(suite->cases[i]->result->msg);
        }
        free(suite->cases[i]->result);
        free(suite->cases[i]);
    }
    free(suite->cases);
    free(suite);
}



int runTestSuite(TestSuite * suite) {
    fprintf(stderr, "Running suite '%s'\n", suite->name);
    fprintf(stdout, "Running suite '%s'\n", suite->name);
    double duration = tick_ms();

    if(suite->count == 0) {
        fprintf(stderr, "  There are no test cases to run, exiting..\n.");
        return 0;
    }

    int fails = 0;

    for(int i = 0; i < suite->count; ++i) {
        TestCase * tc = suite->cases[i];
        fprintf(stderr, "  Running test case %s... ", tc->name);
        fprintf(stdout, "  Running test case %s...\n", tc->name);
        tc->f(tc->result);
        if(tc->result->success) {
            fprintf(stderr, "SUCCESS\n");
        } else {
            fprintf(stderr, "FAILED: %s\n", tc->result->msg);
            fails++;
        }
    }
    duration = tick_ms() - duration;
    fprintf(stderr, "  Suite finished: %d/%d test cases passed and %d/%d failed\n", suite->count - fails, suite->count, fails, suite->count);
    fprintf(stderr, "  Suite took %.3lf ms\n", duration);
    return fails;
}


bool starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void * retry_lib_loading(const char * libname, char const * error) {

    char * pos = strstr(error, "invalid ELF header");
    if(pos) {
        //TODO is this good enough?
        char * b = malloc(sizeof(char) * (strlen(libname) + 3));
        strcpy(b, libname);
        strcat(b, ".0");
        void * lib = dlopen(b, RTLD_NOW | RTLD_GLOBAL);
        free(b);
        return lib;
    }

    return NULL;
}


int main(int argc, char ** argv) {

    argc--;
    argv++;

    if(argc == 0) {
        fprintf(stderr, "Usage: runner [-llib1 -llib2 ...] test_case1.so test_case2.so ...\n");
        return 1;
    }

    atexit(closeLibs);
    int  cases_count = 0;
    char ** libs_names = malloc(argc * sizeof(char *));
    char ** cases_names = malloc(argc * sizeof(char *));

    for(int i = 0; i < argc; ++i) {
        char * arg = argv[i];
        if(starts_with("-l", arg)) {
            char * buf = malloc((/* strlen("lib") */ 3 + strlen(arg) - /* strlen("-l") */ 2 + /* strlen(".so") */ 3 + /* '\0' */ 1) * sizeof(char));
            sprintf(buf, "%s%s%s", "lib", (arg + 2), ".so");
            libs_names[libs_count] = buf;
            libs_count++;
        } else {
            cases_names[cases_count] = arg;
            cases_count++;
        }
    }

    libs = malloc(libs_count * sizeof(void *));

    for(int i = 0; i < libs_count; ++i) {
        fprintf(stderr, "Loading library %s\n", libs_names[i]);
        void * lib = dlopen(libs_names[i], RTLD_NOW | RTLD_GLOBAL);
        if(!lib) {
            const char * error = dlerror();
            lib = retry_lib_loading(libs_names[i], error);
            if(!lib) {
                fprintf(stderr, "Unable to load %s\n", libs_names[i]);
                fprintf(stderr, "%s\n", error);
                return 1;
            }
        }
        libs[i] = lib;
    }

    int total_fails = 0;

    fprintf(stderr, "Starting the test runner. There are %d test suites to run\n", cases_count);
    for(int i = 0; i < cases_count; ++i) {
        char * filename = cases_names[i];
        void * test_case = dlopen(filename, RTLD_LAZY);

        if(!test_case) {
            fprintf(stderr, "Unable to open testcase file: %s\n", filename);
            fprintf(stderr, "%s\n", dlerror());
            return 1;
        }

        TestSuite * (*init)(void) = dlsym(test_case, "init_test_suite");
        char * result = dlerror();
        if(result) {
            fprintf(stderr, "Unable to find initialization code - did you forget to define test suite?\n");
            dlclose(test_case);
            return 1;
        }

        TestSuite * suite = init();
        int fails =  runTestSuite(suite);
        total_fails += fails;
        cleanSuite(suite);

       int ret = dlclose(test_case);
       if(ret) {
           fprintf(stderr, "Failed to close test case %d\n", i);
       }
    }

    for(int i = libs_count - 1; i >= 0; --i) {
        free(libs_names[i]);
    }
    free(libs_names);
    free(cases_names);

    if(total_fails) {
        fprintf(stderr, "FAILURE: %d tests failed\n", total_fails);
        return 1;
    } else {
        fprintf(stderr,"SUCCESS\n");
    }
}


