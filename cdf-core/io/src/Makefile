include ../../Makefile.inc

EXECUTABLE=libcdfio.so
CFLAGS_=$(CFLAGS) -I../../ooc/src
LDFLAGS_=$(LDFLAGS) -shared

SOURCES:=${wildcard *.c}
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS_) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
