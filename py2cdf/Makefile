CC=gcc -std=c17
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
LIBCDF_DIST:=../cdf/dist
CDF_DB_SRC:=../cdf-db/src
CDF_DB_SQLITE_SRC:=../cdf-db-sqlite/src
CDF_HOME ?= $(HOME)/.cdf
CDF_DB_LIB := $(CDF_HOME)/cdf/cdf-db/0.4/lib
CDF_DB_SQLITE_LIB := $(CDF_HOME)/cdf/cdf-db-sqlite/0.4/lib
CFLAGS=-c -Wall -fPIC -Os -pthread
LDFLAGS=-pthread
PY2CDF=$(PROJECT_ROOT)py2cdf.py
BUILD_DIR := $(PROJECT_ROOT)build

EXECUTABLE=libfoo.so
CFLAGS_=$(CFLAGS) -I$(LIBCDF_DIST)/include -I$(CDF_DB_SRC) -I$(CDF_DB_SQLITE_SRC)
LIBS=-L$(LIBCDF_DIST)/lib -L$(CDF_DB_LIB) -L$(CDF_DB_SQLITE_LIB) -lcdfdbsqlite -lcdfdb -lcdfcore -lsqlite3
LDFLAGS_=$(LDFLAGS) -shared $(LIBS)

SOURCES:=${wildcard *.cs}
CSOURCES=$(addprefix $(BUILD_DIR)/, $(SOURCES:.cs=.csc))
OBJECTS=$(CSOURCES:.csc=.o)
GENERATED_HEADERS=$(addprefix $(BUILD_DIR)/, $(SOURCES:.cs=.csh))

all: $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS_) $(OBJECTS) -o $@

$(BUILD_DIR)/%.csc : %.cs $(BUILD_DIR)/%.csh
	mkdir -p $(BUILD_DIR)
	$(PY2CDF) -s -o $(BUILD_DIR) -I $(LIBCDF_DIST)/include $(CDF_DB_SRC) $(CDF_DB_SQLITE_SRC) $(BUILD_DIR) -i $<

$(BUILD_DIR)/%.o : $(BUILD_DIR)/%.csc $(GENERATED_HEADERS)
	$(CC) $(CFLAGS_) -I$(BUILD_DIR) -x c $< -o $@

$(BUILD_DIR)/%.csh : %.cs
	mkdir -p $(BUILD_DIR)
	$(PY2CDF) -h -o $(BUILD_DIR) -i $<

main: $(BUILD_DIR)/$(EXECUTABLE)
	$(CC) $(CFLAGS_) -I$(BUILD_DIR) -o $(BUILD_DIR)/main.o main.c
	$(CC) -o $(BUILD_DIR)/main $(BUILD_DIR)/main.o -L$(BUILD_DIR) -lfoo $(LIBS)

run: main
	@LD_LIBRARY_PATH="$(LIBCDF_DIST)/lib:$(CDF_DB_LIB):$(CDF_DB_SQLITE_LIB):$(BUILD_DIR)" $(BUILD_DIR)/main

valgrind: main
	@LD_LIBRARY_PATH="$(LIBCDF_DIST)/lib:$(CDF_DB_LIB):$(CDF_DB_SQLITE_LIB):$(BUILD_DIR)" valgrind --leak-check=full $(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR) main __pycache__

.SECONDARY: $(GENERATED_HEADERS) $(CSOURCES)
