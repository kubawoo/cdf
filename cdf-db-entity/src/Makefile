include ../Makefile.inc

EXECUTABLE=libcdfdbentity.so
CFLAGS_=$(CFLAGS) -I$(CDF_CORE_PATH)/include -I$(CDF_JSON_PATH)/include -I$(CDF_DB_PATH)/include
LDFLAGS_=$(LDFLAGS) -shared -L$(CDF_CORE_PATH)/lib -L$(CDF_JSON_PATH)/lib -L$(CDF_DB_PATH)/lib -lcdf -lcdfjson -lcdfdb

SOURCES:=${wildcard *.c}
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
