include ../Makefile.inc

all: framework runner

framework: test_framework.c test_framework.h test_framework_types.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC test_framework.c -o $(BUILD_DIR)/test_framework.o
	$(CC) $(LDFLAGS) -shared -o $(BUILD_DIR)/libtestframework.so $(BUILD_DIR)/test_framework.o -lm

runner: runner.c test_framework_types.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) runner.c -o $(BUILD_DIR)/runner.o
	$(CC) $(LDFLAGS) $(BUILD_DIR)/runner.o -o $(BUILD_DIR)/testrunner -ldl -L$(BUILD_DIR) -ltestframework -lrt

clean:
	rm -rf $(BUILD_DIR)

