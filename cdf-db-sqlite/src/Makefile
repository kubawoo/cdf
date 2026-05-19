include ../Makefile.inc

EXECUTABLE=libcdfdbsqlite.so
CFLAGS_=$(CFLAGS) -I$(CDF_CORE_PATH)/include -I$(CDF_DB_PATH)/include
LDFLAGS_=$(LDFLAGS) -shared -L$(CDF_CORE_PATH)/lib -L$(CDF_DB_PATH)/lib -lcdf -lcdfdb

SOURCES:=${wildcard *.c}
OBJECTS=$(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))

all: $(SOURCES) $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf $(BUILD_DIR)
