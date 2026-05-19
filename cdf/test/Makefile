include ../Makefile.inc

all: test_cases

TEST_CASES := ${wildcard tc_*.c}
test_cases:
	$(foreach tc, $(TEST_CASES), $(CC) -g -Wall -fPIC -shared -I../src -I$(TEST_FRAMEWORK_PATH)/include ${tc} -o $(BUILD_DIR)/$(patsubst %.c,%.so,$(tc)); )

test: test_cases
	@LD_LIBRARY_PATH="$(BUILD_DIR):$(TEST_FRAMEWORK_PATH)/lib" $(TEST_RUNNER) -lcdf $(BUILD_DIR)/tc_*.so > output.log

valgrind: test_cases
	@LD_LIBRARY_PATH="$(BUILD_DIR):$(TEST_FRAMEWORK_PATH)/lib" valgrind --error-exitcode=1 --show-leak-kinds=all --leak-check=full $(TEST_RUNNER) -lcdf $(BUILD_DIR)/tc_*.so > output.log
clean:
	rm -rf $(BUILD_DIR)/tc_*.so output.log
