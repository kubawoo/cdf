CC=gcc -std=c17
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
LIBCDF_DIST:=../cdf-core/dist
CFLAGS=-c -Wall -fPIC -Os -pthread
LDFLAGS=-pthread
PY2CDF=$(PROJECT_ROOT)py2cdf.py
BUILD_DIR := $(PROJECT_ROOT)build

EXECUTABLE=libfoo.so
CFLAGS_=$(CFLAGS) -I$(LIBCDF_DIST)/include
LIBS=-L$(LIBCDF_DIST)/lib -lcdfcore -lsqlite3
LDFLAGS_=$(LDFLAGS) -shared $(LIBS)

SOURCES:=${wildcard *.cs}
OBJECTS=$(addprefix $(BUILD_DIR)/, $(SOURCES:.cs=.o))
GENERATED_HEADERS=$(SOURCES:.cs=.csh)

all: $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o : %.cs $(GENERATED_HEADERS)
	mkdir -p $(BUILD_DIR)
	$(PY2CDF) -s -I $(LIBCDF_DIST)/include . -i $<
	$(CC) $(CFLAGS_) -x c $<c -o $@

%.csh : %.cs
	$(PY2CDF) -h -i $<

main: $(BUILD_DIR)/$(EXECUTABLE)
	$(CC) $(CFLAGS_) -o $(BUILD_DIR)/main.o main.c
	$(CC) -o $(BUILD_DIR)/main $(BUILD_DIR)/main.o -L$(BUILD_DIR) -lfoo $(LIBS)

run: main
	@LD_LIBRARY_PATH="$(LIBCDF_DIST)/lib:$(BUILD_DIR)" ./$(BUILD_DIR)/main

valgrind: main
	@LD_LIBRARY_PATH="$(LIBCDF_DIST)/lib:$(BUILD_DIR)" valgrind --leak-check=full ./$(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR) *.csc *.csh main __pycache__

.SECONDARY: $(GENERATED_HEADERS)
