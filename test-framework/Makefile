include Makefile.inc

.PHONY: build
.PHONY: test

all: dist

build:
	make -C src

clean:
	rm -rf dist
	make -C src clean
	make -C test clean

test: dist
	make -C test test

valgrind: build
	make -C test valgrind

dist: build
	mkdir -p dist/bin
	cp src/testrunner dist/bin
	mkdir -p dist/include
	cp src/test_framework.h dist/include
	cp src/test_framework_types.h dist/include
	mkdir -p dist/lib
	cp src/libtestframework.so dist/lib

install: dist
	$(info Installing to $(MOD_INSTALL_PATH))
	rm -rf $(MOD_INSTALL_PATH)
	mkdir -p $(MOD_INSTALL_PATH)
	cp -r dist/* $(MOD_INSTALL_PATH)
	cp cdfmodule.json $(MOD_INSTALL_PATH)