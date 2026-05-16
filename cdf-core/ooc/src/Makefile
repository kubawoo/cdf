include ../../Makefile.inc

EXECUTABLE=libcdfooc.so
CFLAGS_=$(CFLAGS)
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
