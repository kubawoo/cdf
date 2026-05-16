include ../Makefile.inc

EXECUTABLE=wwwserver
CFLAGS_=$(CFLAGS) -I$(CDF_CORE_PATH)/include -I$(CDF_HTTP_PATH)/include
LDFLAGS_=$(LDFLAGS) -L$(CDF_CORE_PATH)/lib -lcdfcore -L$(CDF_HTTP_PATH)/lib -lcdfhttp
EXAMPLE_DIR := wwwserver

SOURCES:=${wildcard *.c}
OBJECTS=$(addprefix $(BUILD_DIR)/$(EXAMPLE_DIR)/, $(SOURCES:.c=.o))

all: $(SOURCES) $(BUILD_DIR)/$(EXAMPLE_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXAMPLE_DIR)/$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CC) $(OBJECTS) $(LDFLAGS_) -o $@

$(BUILD_DIR)/$(EXAMPLE_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/$(EXAMPLE_DIR)

run: all
	@LD_LIBRARY_PATH="$(CDF_CORE_PATH)/lib:$(CDF_HTTP_PATH)/lib" $(BUILD_DIR)/$(EXAMPLE_DIR)/$(EXECUTABLE)

valgrind: all
	@LD_LIBRARY_PATH="$(CDF_CORE_PATH)/lib:$(CDF_HTTP_PATH)/lib" valgrind --leak-check=full $(BUILD_DIR)/$(EXAMPLE_DIR)/$(EXECUTABLE)

