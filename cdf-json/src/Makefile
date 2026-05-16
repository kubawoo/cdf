include ../Makefile.inc

EXECUTABLE=libcdfjson.so
CFLAGS_=$(CFLAGS) -I$(CDF_CORE_PATH)/include
LDFLAGS_=$(LDFLAGS) -shared -L$(CDF_CORE_PATH)/lib -lcdf

SOURCES:=${wildcard *.c}
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
