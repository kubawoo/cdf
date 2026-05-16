include ../../Makefile.inc

EXECUTABLE=libcdfio.so
CFLAGS_=$(CFLAGS) -I../../ooc/src
LDFLAGS_=$(LDFLAGS) -shared
MODULE_NAME=io

SOURCES:=${wildcard *.c}
OBJECTS=$(addprefix $(BUILD_DIR)/$(MODULE_NAME)/, $(SOURCES:.c=.o))

all: $(SOURCES) $(BUILD_DIR)/$(MODULE_NAME)/$(EXECUTABLE)

$(BUILD_DIR)/$(MODULE_NAME)/$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

$(BUILD_DIR)/$(MODULE_NAME)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/$(MODULE_NAME)
