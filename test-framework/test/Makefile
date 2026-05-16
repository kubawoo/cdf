include ../Makefile.inc

all: test_cases

TEST_CASES := ${wildcard tc_*.c}
test_cases:
	$(foreach tc, $(TEST_CASES), $(CC) -g -Wall -fPIC -shared -I../dist/include ${tc} -o $(patsubst %.c,%.so,$(tc)); )

test: test_cases
	@LD_LIBRARY_PATH=".:../dist/lib" ../dist//bin/testrunner tc_*.so > output.log

valgrind: test_cases
	@LD_LIBRARY_PATH=".:../dist/lib" valgrind --show-leak-kinds=all --leak-check=full ../dist/bin/testrunner tc_*.so > output.log
clean:
	rm -rf *.o *.so output.log
