include ../Makefile.inc

EXECUTABLE=cdftk
CFLAGS_=$(CFLAGS) -I$(CDF_CORE_PATH)/include -I$(CDF_JSON_PATH)/include
LDFLAGS_=$(LDFLAGS) -L$(CDF_CORE_PATH)/lib -lcdfcore -Wl,-rpath=$(CDF_CORE_PATH)/lib -L$(CDF_JSON_PATH)/lib -lcdfjson -Wl,-rpath=$(CDF_JSON_PATH)/lib

SOURCES:=${wildcard *.c}
OBJECTS=$(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))

all: $(SOURCES) $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS_) -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run: all
	@LD_LIBRARY_PATH="$(CDF_CORE_PATH)/lib" $(BUILD_DIR)/$(EXECUTABLE)

valgrind: all
	@LD_LIBRARY_PATH="$(CDF_CORE_PATH)/lib" valgrind --leak-check=full $(BUILD_DIR)/$(EXECUTABLE)

