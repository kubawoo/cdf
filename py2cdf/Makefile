CC=c99
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
LIBCDF_DIST:=../cdf/dist
CFLAGS=-c -Wall -fPIC -Os -pthread
LDFLAGS=-pthread
PY2CDF=$(PROJECT_ROOT)py2cdf.py

EXECUTABLE=libfoo.so
CFLAGS_=$(CFLAGS) -I$(LIBCDF_DIST)/include
LIBS=-L$(LIBCDF_DIST)/lib -lcdfcore -lsqlite3
LDFLAGS_=$(LDFLAGS) -shared $(LIBS)

SOURCES:=${wildcard *.cs}
OBJECTS=$(SOURCES:.cs=.o)
GENERATED_HEADERS=$(SOURCES:.cs=.csh)


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

%.o : %.cs $(GENERATED_HEADERS)
	$(PY2CDF) -s -I $(LIBCDF_DIST)/include . -i $<
	$(CC) $(CFLAGS_) -x c  $<c -o $@

%.csh : %.cs
	$(PY2CDF) -h -i $<


main: $(EXECUTABLE)
	$(CC) $(CFLAGS_) -o main.o main.c
	$(CC) -o main main.o -L. -lfoo $(LIBS)

run: main
	@LD_LIBRARY_PATH="$(LIBCDF_DIST)/lib:." ./main

valgrind: main
	@LD_LIBRARY_PATH="$(LIBCDF_DIST)/lib:." valgrind --leak-check=full ./main

clean:
	rm -rf *.o *.csc *.csh main __pycache__ $(EXECUTABLE)

.SECONDARY: $(GENERATED_HEADERS)
