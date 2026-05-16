include ../../Makefile.inc

EXECUTABLE=libcdfooc.so
CFLAGS_=$(CFLAGS)
LDFLAGS_=$(LDFLAGS) -shared
MODULE_NAME=ooc

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
