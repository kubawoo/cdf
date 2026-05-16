include ../Makefile.inc

EXECUTABLE=cdftk
CFLAGS_=$(CFLAGS) -I$(CDF_CORE_PATH)/include -I$(CDF_JSON_PATH)/include
LDFLAGS_=$(LDFLAGS) -L$(CDF_CORE_PATH)/lib -lcdf -Wl,-rpath=$(CDF_CORE_PATH)/lib -L$(CDF_JSON_PATH)/lib -lcdfjson -Wl,-rpath=$(CDF_JSON_PATH)/lib

SOURCES:=${wildcard *.c}
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS_) -o $@

.c.o:
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

run: all
	@LD_LIBRARY_PATH="$(CDF_CORE_PATH)/lib" ./$(EXECUTABLE)

valgrind: all
	@LD_LIBRARY_PATH="$(CDF_CORE_PATH)/lib" valgrind --leak-check=full ./$(EXECUTABLE)

