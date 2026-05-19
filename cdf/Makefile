include Makefile.inc

all: dist

.PHONY: build
.PHONY: test

build:
	make -C src

test: build
	make -C test test

valgrind: build
	make -C test valgrind

clean:
	rm -rf dist
	make -C src clean
	make -C test clean


dist: build
	mkdir -p dist/lib
	mkdir -p dist/include
	cp src/*.h dist/include
	cp $(BUILD_DIR)/*.so dist/lib

install: dist
	$(info Installing to $(MOD_INSTALL_PATH))
	rm -rf $(MOD_INSTALL_PATH)
	mkdir -p $(MOD_INSTALL_PATH)
	cp -r dist/* $(MOD_INSTALL_PATH)
	cp cdfmodule.json $(MOD_INSTALL_PATH)
