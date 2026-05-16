include ../Makefile.inc

all: test_cases

TEST_CASES := ${wildcard tc_*.c}
test_cases:
	$(foreach tc, $(TEST_CASES), $(CC) -g -Wall -fPIC -shared -I../src -I$(CDF_CORE_PATH)/include -I$(TEST_FRAMEWORK_PATH)/include ${tc} -o $(patsubst %.c,%.so,$(tc)); )

test: test_cases
	@LD_LIBRARY_PATH=".:../src:$(CDF_CORE_PATH)/lib:$(TEST_FRAMEWORK_PATH)/lib" $(TEST_RUNNER) -lcdfcore -lcdfjson tc_*.so > output.log

valgrind: test_cases
	@LD_LIBRARY_PATH=".:../src:$(CDF_CORE_PATH)/lib:$(TEST_FRAMEWORK_PATH)/lib" valgrind --show-leak-kinds=all --leak-check=full $(TEST_RUNNER) -lcdfcore -lcdfjson tc_*.so > output.log
clean:
	rm -rf *.o *.so output.log
