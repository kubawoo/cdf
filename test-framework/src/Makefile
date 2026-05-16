include ../Makefile.inc

all: framework runner

framework: test_framework.c test_framework.h test_framework_types.h
	$(CC) $(CFLAGS) -fPIC test_framework.c
	$(CC) $(LDFLAGS) -shared -o libtestframework.so test_framework.o -lm

runner: runner.c test_framework_types.h
	$(CC) $(CFLAGS) runner.c
	$(CC) $(LDFLAGS) runner.o -o testrunner -ldl -L. -ltestframework -lrt

clean:
	rm -rf *.o testrunner libtestframework.so

